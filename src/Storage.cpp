/**
 * @file Storage.cpp
 * @brief Implementation of LittleFS storage management
 *
 * Implementacja zarządzania pamięcią LittleFS
 */

#include "Storage.h"

bool Storage::initialized = false;

bool Storage::begin() {
    if (initialized) {
        return true;
    }

    Serial.println("[Storage] Initializing LittleFS...");

    if (!LittleFS.begin(STORAGE_FORMAT_ON_FAIL)) {
        Serial.println("[Storage] ERROR: LittleFS mount failed!");
        return false;
    }

    initialized = true;
    Serial.println("[Storage] LittleFS initialized successfully");

    // Print storage info / Wyświetl info o pamięci
    size_t total, used;
    getStorageInfo(total, used);
    Serial.printf("[Storage] Total: %u bytes, Used: %u bytes, Free: %u bytes\n",
                  total, used, total - used);

    return true;
}

bool Storage::format() {
    Serial.println("[Storage] WARNING: Formatting LittleFS - all data will be lost!");
    if (!LittleFS.format()) {
        Serial.println("[Storage] ERROR: Format failed!");
        return false;
    }
    Serial.println("[Storage] Format successful");
    return true;
}

// ============================================================================
// WiFi Configuration
// ============================================================================

bool Storage::loadWiFiConfig(WiFiConfig& config) {
    JsonDocument doc;

    if (!readJsonFile(CONFIG_FILE_PATH, doc)) {
        Serial.println("[Storage] No WiFi config found, using defaults");
        config.ssid = "";
        config.password = "";
        config.configured = false;
        return false;
    }

    if (doc.containsKey("wifi")) {
        JsonObject wifi = doc["wifi"];
        config.ssid = wifi["ssid"].as<String>();
        config.password = wifi["password"].as<String>();

        // Basic decryption if enabled / Podstawowa deszyfracja jeśli włączona
        if (SECURITY_ENCRYPT_PASSWORDS && config.password.length() > 0) {
            config.password = xorEncrypt(config.password, ENCRYPTION_KEY);
        }

        config.configured = (config.ssid.length() > 0);
        Serial.printf("[Storage] WiFi config loaded: SSID='%s'\n", config.ssid.c_str());
        return true;
    }

    return false;
}

bool Storage::saveWiFiConfig(const WiFiConfig& config) {
    JsonDocument doc;

    // Load existing config or create new / Wczytaj istniejącą lub utwórz nową
    readJsonFile(CONFIG_FILE_PATH, doc);

    // Encrypt password if enabled / Zaszyfruj hasło jeśli włączone
    String password = config.password;
    if (SECURITY_ENCRYPT_PASSWORDS && password.length() > 0) {
        password = xorEncrypt(password, ENCRYPTION_KEY);
    }

    // Update WiFi section / Zaktualizuj sekcję WiFi
    JsonObject wifi = doc["wifi"].to<JsonObject>();
    wifi["ssid"] = config.ssid;
    wifi["password"] = password;
    wifi["configured"] = config.configured;

    bool success = writeJsonFile(CONFIG_FILE_PATH, doc);
    if (success) {
        Serial.printf("[Storage] WiFi config saved: SSID='%s'\n", config.ssid.c_str());
    }
    return success;
}

// ============================================================================
// Device Management
// ============================================================================

bool Storage::loadDevices(std::vector<ShellyDevice>& devices) {
    JsonDocument doc;

    if (!readJsonFile(DEVICES_FILE_PATH, doc)) {
        Serial.println("[Storage] No devices file found");
        return false;
    }

    devices.clear();
    JsonArray devicesArray = doc["devices"].as<JsonArray>();

    for (JsonObject deviceObj : devicesArray) {
        ShellyDevice device;
        device.id = deviceObj["id"].as<String>();
        device.name = deviceObj["name"].as<String>();
        device.mac = deviceObj["mac"].as<String>();
        device.ip = deviceObj["ip"].as<String>();
        device.enabled = deviceObj["enabled"] | true;
        device.lastSeen = deviceObj["lastSeen"] | 0;
        device.lastPower = deviceObj["lastPower"] | 0.0f;

        devices.push_back(device);
    }

    Serial.printf("[Storage] Loaded %d devices\n", devices.size());
    return true;
}

bool Storage::saveDevices(const std::vector<ShellyDevice>& devices) {
    JsonDocument doc;
    JsonArray devicesArray = doc["devices"].to<JsonArray>();

    for (const auto& device : devices) {
        JsonObject deviceObj = devicesArray.add<JsonObject>();
        deviceObj["id"] = device.id;
        deviceObj["name"] = device.name;
        deviceObj["mac"] = device.mac;
        deviceObj["ip"] = device.ip;
        deviceObj["enabled"] = device.enabled;
        deviceObj["lastSeen"] = device.lastSeen;
        deviceObj["lastPower"] = device.lastPower;
    }

    bool success = writeJsonFile(DEVICES_FILE_PATH, doc);
    if (success) {
        Serial.printf("[Storage] Saved %d devices\n", devices.size());
    }
    return success;
}

bool Storage::updateDevice(const ShellyDevice& device) {
    std::vector<ShellyDevice> devices;
    loadDevices(devices);

    bool found = false;
    for (auto& dev : devices) {
        if (dev.id == device.id || dev.mac == device.mac) {
            dev = device;
            found = true;
            break;
        }
    }

    if (!found) {
        devices.push_back(device);
    }

    return saveDevices(devices);
}

bool Storage::findDeviceByMAC(const String& mac, ShellyDevice& device) {
    std::vector<ShellyDevice> devices;
    if (!loadDevices(devices)) {
        return false;
    }

    for (const auto& dev : devices) {
        if (dev.mac.equalsIgnoreCase(mac)) {
            device = dev;
            return true;
        }
    }

    return false;
}

// ============================================================================
// Rules Management
// ============================================================================

bool Storage::loadRules(std::vector<LogicRule>& rules) {
    JsonDocument doc;

    if (!readJsonFile(RULES_FILE_PATH, doc)) {
        Serial.println("[Storage] No rules file found");
        return false;
    }

    rules.clear();
    JsonArray rulesArray = doc["rules"].as<JsonArray>();

    for (JsonObject ruleObj : rulesArray) {
        LogicRule rule;
        rule.id = ruleObj["id"].as<String>();
        rule.deviceId = ruleObj["deviceId"].as<String>();
        rule.threshold = ruleObj["threshold"] | POWER_EXPORT_THRESHOLD;
        rule.hysteresis = ruleObj["hysteresis"] | HYSTERESIS_DEFAULT;
        rule.delayOn = ruleObj["delayOn"] | DELAY_BEFORE_ON;
        rule.delayOff = ruleObj["delayOff"] | DELAY_BEFORE_OFF;
        rule.priority = ruleObj["priority"] | 0;
        rule.enabled = ruleObj["enabled"] | true;
        rule.condition = ruleObj["condition"] | "export_above";

        rules.push_back(rule);
    }

    Serial.printf("[Storage] Loaded %d rules\n", rules.size());
    return true;
}

bool Storage::saveRules(const std::vector<LogicRule>& rules) {
    JsonDocument doc;
    JsonArray rulesArray = doc["rules"].to<JsonArray>();

    for (const auto& rule : rules) {
        JsonObject ruleObj = rulesArray.add<JsonObject>();
        ruleObj["id"] = rule.id;
        ruleObj["deviceId"] = rule.deviceId;
        ruleObj["threshold"] = rule.threshold;
        ruleObj["hysteresis"] = rule.hysteresis;
        ruleObj["delayOn"] = rule.delayOn;
        ruleObj["delayOff"] = rule.delayOff;
        ruleObj["priority"] = rule.priority;
        ruleObj["enabled"] = rule.enabled;
        ruleObj["condition"] = rule.condition;
    }

    bool success = writeJsonFile(RULES_FILE_PATH, doc);
    if (success) {
        Serial.printf("[Storage] Saved %d rules\n", rules.size());
    }
    return success;
}

// ============================================================================
// Backend Configuration
// ============================================================================

bool Storage::loadBackendConfig(BackendConfig& config) {
    JsonDocument doc;

    if (!readJsonFile(CONFIG_FILE_PATH, doc)) {
        Serial.println("[Storage] No backend config found");
        return false;
    }

    if (doc.containsKey("backend")) {
        JsonObject backend = doc["backend"];
        config.baseUrl = backend["baseUrl"].as<String>();
        config.username = backend["username"].as<String>();
        config.password = backend["password"].as<String>();

        // Decrypt password if enabled / Odszyfruj hasło jeśli włączone
        if (SECURITY_ENCRYPT_PASSWORDS && config.password.length() > 0) {
            config.password = xorEncrypt(config.password, ENCRYPTION_KEY);
        }

        config.token = backend["token"].as<String>();
        config.useHttps = backend["useHttps"] | false;
        config.syncInterval = backend["syncInterval"] | REST_SYNC_INTERVAL;

        Serial.printf("[Storage] Backend config loaded: URL='%s'\n", config.baseUrl.c_str());
        return true;
    }

    return false;
}

bool Storage::saveBackendConfig(const BackendConfig& config) {
    JsonDocument doc;

    // Load existing config / Wczytaj istniejącą konfigurację
    readJsonFile(CONFIG_FILE_PATH, doc);

    // Encrypt password if enabled / Zaszyfruj hasło jeśli włączone
    String password = config.password;
    if (SECURITY_ENCRYPT_PASSWORDS && password.length() > 0) {
        password = xorEncrypt(password, ENCRYPTION_KEY);
    }

    // Update backend section / Zaktualizuj sekcję backendu
    JsonObject backend = doc["backend"].to<JsonObject>();
    backend["baseUrl"] = config.baseUrl;
    backend["username"] = config.username;
    backend["password"] = password;
    backend["token"] = config.token;
    backend["useHttps"] = config.useHttps;
    backend["syncInterval"] = config.syncInterval;

    bool success = writeJsonFile(CONFIG_FILE_PATH, doc);
    if (success) {
        Serial.printf("[Storage] Backend config saved: URL='%s'\n", config.baseUrl.c_str());
    }
    return success;
}

// ============================================================================
// State Management
// ============================================================================

bool Storage::saveState(const JsonDocument& state) {
    return writeJsonFile(STATE_FILE_PATH, state);
}

bool Storage::loadState(JsonDocument& state) {
    return readJsonFile(STATE_FILE_PATH, state);
}

// ============================================================================
// Utility Functions
// ============================================================================

bool Storage::fileExists(const String& path) {
    return LittleFS.exists(path);
}

long Storage::getFileSize(const String& path) {
    if (!fileExists(path)) {
        return -1;
    }

    File file = LittleFS.open(path, "r");
    if (!file) {
        return -1;
    }

    long size = file.size();
    file.close();
    return size;
}

void Storage::listFiles() {
    Serial.println("[Storage] Listing files:");
    File root = LittleFS.open("/");
    File file = root.openNextFile();

    while (file) {
        Serial.printf("  %s (%d bytes)\n", file.name(), file.size());
        file = root.openNextFile();
    }
}

void Storage::getStorageInfo(size_t& totalBytes, size_t& usedBytes) {
    totalBytes = LittleFS.totalBytes();
    usedBytes = LittleFS.usedBytes();
}

// ============================================================================
// Private Helper Functions
// ============================================================================

bool Storage::readJsonFile(const String& path, JsonDocument& doc) {
    if (!fileExists(path)) {
        return false;
    }

    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.printf("[Storage] ERROR: Cannot open file for reading: %s\n", path.c_str());
        return false;
    }

    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.printf("[Storage] ERROR: JSON deserialization failed: %s\n", error.c_str());
        return false;
    }

    return true;
}

bool Storage::writeJsonFile(const String& path, const JsonDocument& doc) {
    File file = LittleFS.open(path, "w");
    if (!file) {
        Serial.printf("[Storage] ERROR: Cannot open file for writing: %s\n", path.c_str());
        return false;
    }

    size_t bytesWritten = serializeJson(doc, file);
    file.close();

    if (bytesWritten == 0) {
        Serial.printf("[Storage] ERROR: Failed to write JSON to file: %s\n", path.c_str());
        return false;
    }

    return true;
}

String Storage::xorEncrypt(const String& data, const String& key) {
    String result = "";
    int keyLen = key.length();

    for (size_t i = 0; i < data.length(); i++) {
        result += char(data[i] ^ key[i % keyLen]);
    }

    return result;
}
