/**
 * @file main.cpp
 * @brief Pistachio Smart Controller - Main Application
 *
 * ESP32-based smart controller for Shelly devices with auto-discovery
 * and rule-based power management
 *
 * Główna aplikacja kontrolera inteligentnego Pistachio
 */

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "Storage.h"
#include "WiFiManager.h"
#include "RESTClient.h"
#include "ShellyClient.h"
#include "Discovery.h"
#include "LogicEngine.h"

// ============================================================================
// Global Objects / Obiekty globalne
// ============================================================================
WiFiManager wifiManager;
RESTClient restClient;
Discovery discovery;
LogicEngine logicEngine;
AsyncWebServer* webServer = nullptr;

// ============================================================================
// State Variables / Zmienne stanu
// ============================================================================
unsigned long lastTelemetrySync = 0;
unsigned long lastBackendSync = 0;
unsigned long lastDevicePoll = 0;
bool systemReady = false;
bool webServerStarted = false;
WiFiManagerState lastWiFiState = WiFiManagerState::DISCONNECTED;

// ============================================================================
// Function Prototypes / Prototypy funkcji
// ============================================================================
void setupWebRoutes();
void handleDevicePolling();
void handleTelemetry();
void handleBackendSync();
void setupOTA();
void printSystemInfo();
void restartWebServer();

/**
 * @brief Setup function - runs once at startup
 * Funkcja setup - uruchamiana raz przy starcie
 */
void setup() {
    Serial.begin(DEBUG_SERIAL_BAUD);
    delay(1000);

    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  Pistachio Smart Controller");
    Serial.println("  ESP32 Shelly Manager");
    Serial.printf("  Version: %s\n", FIRMWARE_VERSION);
    Serial.printf("  Build: %s %s\n", BUILD_DATE, BUILD_TIME);
    Serial.println("========================================\n");

    // Initialize status LED / Inicjalizacja LED statusu
    if (STATUS_LED_ENABLE) {
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, LED_ACTIVE_LOW ? HIGH : LOW);
    }

    // Initialize storage / Inicjalizacja pamięci
    Serial.println("[Main] Initializing storage...");
    if (!Storage::begin()) {
        Serial.println("[Main] FATAL: Storage initialization failed!");
        Serial.println("[Main] System will restart in 10 seconds...");
        delay(10000);
        ESP.restart();
    }

    Storage::listFiles();

    // Initialize WiFi Manager / Inicjalizacja menedżera WiFi
    Serial.println("[Main] Initializing WiFi...");
    if (!wifiManager.begin()) {
        Serial.println("[Main] WARNING: WiFi initialization failed!");
    }

    // Wait for WiFi connection or AP mode / Czekaj na połączenie WiFi lub tryb AP
    unsigned long wifiTimeout = millis() + 30000;
    while (!wifiManager.isConnected() && !wifiManager.isAPMode() &&
           millis() < wifiTimeout) {
        wifiManager.loop();
        delay(100);
    }

    if (wifiManager.isConnected()) {
        Serial.println("[Main] Connected to WiFi");
        printSystemInfo();

        // Initialize REST client / Inicjalizacja klienta REST
        BackendConfig backendConfig;
        if (Storage::loadBackendConfig(backendConfig)) {
            restClient.begin(backendConfig);
            Serial.println("[Main] REST client initialized");
        }

        // Initialize discovery / Inicjalizacja odkrywania
        discovery.begin();

        // Initialize logic engine / Inicjalizacja silnika logiki
        logicEngine.begin();

        // Setup OTA / Konfiguracja OTA
        if (OTA_ENABLE) {
            setupOTA();
        }

        systemReady = true;
        Serial.println("[Main] System ready!");

    } else if (wifiManager.isAPMode()) {
        Serial.println("[Main] Running in AP configuration mode");
    }

    // Setup web server / Konfiguracja serwera WWW
    webServer = wifiManager.getWebServer();
    if (webServer) {
        setupWebRoutes();
        webServer->begin();
        webServerStarted = true;
        lastWiFiState = wifiManager.getState();
        Serial.println("[Main] Web server started");
    }

    // LED on to indicate ready / LED włączony oznacza gotowość
    if (STATUS_LED_ENABLE) {
        digitalWrite(LED_PIN, LED_ACTIVE_LOW ? LOW : HIGH);
    }
}

/**
 * @brief Main loop - runs continuously
 * Główna pętla - działa ciągle
 */
void loop() {
    // WiFi manager loop / Pętla menedżera WiFi
    wifiManager.loop();

    // Check if WiFi state changed and restart web server if needed
    // Sprawdź czy stan WiFi się zmienił i zrestartuj serwer jeśli potrzeba
    WiFiManagerState currentState = wifiManager.getState();
    if (currentState != lastWiFiState) {
        Serial.printf("[Main] WiFi state changed: %d -> %d\n", lastWiFiState, currentState);
        lastWiFiState = currentState;

        // Restart web server when transitioning from AP to Connected
        // Restartuj serwer przy przejściu z AP do Connected
        if (currentState == WiFiManagerState::CONNECTED && webServerStarted) {
            Serial.println("[Main] Restarting web server for new IP...");
            restartWebServer();
        }
    }

    if (systemReady && wifiManager.isConnected()) {
        // Device polling / Odpytywanie urządzeń
        handleDevicePolling();

        // Logic engine / Silnik logiki
        logicEngine.loop();

        // Discovery / Odkrywanie
        discovery.periodicDiscovery();

        // Telemetry / Telemetria
        handleTelemetry();

        // Backend sync / Synchronizacja z backendem
        handleBackendSync();

        // OTA handle / Obsługa OTA
        if (OTA_ENABLE) {
            ArduinoOTA.handle();
        }
    }

    // Small delay to prevent watchdog / Małe opóźnienie aby uniknąć watchdoga
    delay(LOOP_DELAY);
}

/**
 * @brief Setup web server routes
 * Konfiguracja tras serwera WWW
 */
void setupWebRoutes() {
    // NOTE: API routes MUST be defined BEFORE serveStatic to have priority
    // UWAGA: Trasy API MUSZĄ być zdefiniowane PRZED serveStatic aby miały priorytet

    // API: System status / API: Status systemu
    webServer->on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument doc;
        doc["version"] = FIRMWARE_VERSION;
        doc["uptime"] = millis() / 1000;
        doc["wifi"]["connected"] = wifiManager.isConnected();
        doc["wifi"]["ssid"] = wifiManager.getSSID();
        doc["wifi"]["ip"] = wifiManager.getIPAddress();
        doc["wifi"]["rssi"] = wifiManager.getRSSI();
        doc["wifi"]["mac"] = wifiManager.getMACAddress();

        size_t total, used;
        Storage::getStorageInfo(total, used);
        doc["storage"]["total"] = total;
        doc["storage"]["used"] = used;
        doc["storage"]["free"] = total - used;

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: Save WiFi configuration / API: Zapisz konfigurację WiFi
    webServer->on("/api/wifi/config", HTTP_POST, [](AsyncWebServerRequest *request) {},
                  NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                          size_t index, size_t total) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
            request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        String ssid = doc["ssid"].as<String>();
        String password = doc["password"].as<String>();

        if (ssid.length() == 0) {
            request->send(400, "application/json", "{\"error\":\"SSID required\"}");
            return;
        }

        // Save and connect / Zapisz i połącz
        WiFiConfig config;
        config.ssid = ssid;
        config.password = password;
        config.configured = true;
        Storage::saveWiFiConfig(config);

        request->send(200, "application/json", "{\"success\":true}");

        // Attempt connection after response / Spróbuj połączyć po odpowiedzi
        delay(1000);
        wifiManager.connectToWiFi(ssid, password);
    });

    // API: Get devices / API: Pobierz urządzenia
    webServer->on("/api/devices", HTTP_GET, [](AsyncWebServerRequest *request) {
        std::vector<ShellyDevice> devices;
        Storage::loadDevices(devices);

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

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: Add device manually / API: Dodaj urządzenie ręcznie
    webServer->on("/api/device/add", HTTP_POST, [](AsyncWebServerRequest *request) {},
                  NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                          size_t index, size_t total) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
            request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        String mac = doc["mac"].as<String>();
        String name = doc["name"].as<String>();

        if (mac.length() == 0) {
            request->send(400, "application/json", "{\"error\":\"MAC address required\"}");
            return;
        }

        if (name.length() == 0) {
            name = "Shelly Device";
        }

        // Normalize MAC address format (uppercase, with colons)
        // Normalizuj format MAC (wielkie litery, z dwukropkami)
        mac.toUpperCase();

        // Create device / Utwórz urządzenie
        ShellyDevice device;
        device.mac = mac;
        device.name = name;
        device.id = "shelly_" + mac;
        device.id.replace(":", "");
        device.ip = ""; // Will be discovered / Zostanie odkryte
        device.enabled = true;
        device.lastSeen = 0;
        device.lastPower = 0.0f;

        // Check if device already exists / Sprawdź czy urządzenie już istnieje
        std::vector<ShellyDevice> devices;
        Storage::loadDevices(devices);

        bool exists = false;
        for (const auto& dev : devices) {
            if (dev.mac.equalsIgnoreCase(mac)) {
                exists = true;
                break;
            }
        }

        if (exists) {
            request->send(400, "application/json", "{\"error\":\"Device with this MAC already exists\"}");
            return;
        }

        // Add device / Dodaj urządzenie
        devices.push_back(device);
        Storage::saveDevices(devices);

        Serial.printf("[Main] Device added: %s (%s)\n", name.c_str(), mac.c_str());

        // Trigger discovery to find IP / Uruchom discovery aby znaleźć IP
        JsonDocument response;
        response["success"] = true;
        response["message"] = "Device added. Discovery will find IP address automatically.";
        response["device"]["id"] = device.id;
        response["device"]["mac"] = device.mac;
        response["device"]["name"] = device.name;

        String responseStr;
        serializeJson(response, responseStr);
        request->send(200, "application/json", responseStr);
    });

    // API: Control device / API: Sterowanie urządzeniem
    webServer->on("/api/device/control", HTTP_POST, [](AsyncWebServerRequest *request) {},
                  NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                          size_t index, size_t total) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
            request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        String deviceId = doc["deviceId"].as<String>();
        bool state = doc["state"] | false;

        bool success = logicEngine.manualControl(deviceId, state);

        if (success) {
            request->send(200, "application/json", "{\"success\":true}");
        } else {
            request->send(500, "application/json", "{\"error\":\"Control failed\"}");
        }
    });

    // API: Get rules / API: Pobierz reguły
    webServer->on("/api/rules", HTTP_GET, [](AsyncWebServerRequest *request) {
        std::vector<LogicRule> rules;
        Storage::loadRules(rules);

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

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: Start discovery scan / API: Uruchom skanowanie odkrywania
    webServer->on("/api/discovery/scan", HTTP_POST, [](AsyncWebServerRequest *request) {
        std::vector<DiscoveredDevice> devices;
        int count = discovery.scanNetwork(devices);
        discovery.updateDeviceList(devices);

        JsonDocument doc;
        doc["discovered"] = count;

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: Delete device / API: Usuń urządzenie
    webServer->on("/api/device/delete", HTTP_POST, [](AsyncWebServerRequest *request) {},
                  NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                          size_t index, size_t total) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
            request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        String deviceId = doc["id"].as<String>();

        std::vector<ShellyDevice> devices;
        Storage::loadDevices(devices);

        // Remove device / Usuń urządzenie
        auto it = std::remove_if(devices.begin(), devices.end(),
            [&deviceId](const ShellyDevice& dev) { return dev.id == deviceId; });

        if (it != devices.end()) {
            devices.erase(it, devices.end());
            Storage::saveDevices(devices);

            request->send(200, "application/json", "{\"success\":true}");
        } else {
            request->send(404, "application/json", "{\"error\":\"Device not found\"}");
        }
    });

    // API: Export configuration / API: Eksportuj konfigurację
    webServer->on("/api/config/export", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument doc;

        // WiFi config / Konfiguracja WiFi
        WiFiConfig wifiConfig;
        if (Storage::loadWiFiConfig(wifiConfig)) {
            JsonObject wifi = doc["wifi"].to<JsonObject>();
            wifi["ssid"] = wifiConfig.ssid;
            wifi["configured"] = wifiConfig.configured;
            // Don't export password for security / Nie eksportuj hasła ze względów bezpieczeństwa
        }

        // Devices / Urządzenia
        std::vector<ShellyDevice> devices;
        Storage::loadDevices(devices);

        JsonArray devicesArray = doc["devices"].to<JsonArray>();
        for (const auto& device : devices) {
            JsonObject deviceObj = devicesArray.add<JsonObject>();
            deviceObj["id"] = device.id;
            deviceObj["name"] = device.name;
            deviceObj["mac"] = device.mac;
            deviceObj["ip"] = device.ip;
            deviceObj["enabled"] = device.enabled;
        }

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: Import devices configuration / API: Importuj konfigurację urządzeń
    webServer->on("/api/config/import", HTTP_POST, [](AsyncWebServerRequest *request) {},
                  NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                          size_t index, size_t total) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
            request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        int imported = 0;

        // Import devices / Importuj urządzenia
        if (doc.containsKey("devices")) {
            std::vector<ShellyDevice> devices;
            Storage::loadDevices(devices);

            JsonArray devicesArray = doc["devices"].as<JsonArray>();
            for (JsonObject deviceObj : devicesArray) {
                ShellyDevice device;
                device.id = deviceObj["id"].as<String>();
                device.name = deviceObj["name"].as<String>();
                device.mac = deviceObj["mac"].as<String>();
                device.ip = deviceObj["ip"] | "";
                device.enabled = deviceObj["enabled"] | true;
                device.lastSeen = 0;
                device.lastPower = 0.0f;

                // Check for duplicates / Sprawdź duplikaty
                bool exists = false;
                for (const auto& dev : devices) {
                    if (dev.mac == device.mac || dev.id == device.id) {
                        exists = true;
                        break;
                    }
                }

                if (!exists && device.mac.length() > 0) {
                    devices.push_back(device);
                    imported++;
                }
            }

            Storage::saveDevices(devices);
        }

        JsonDocument response;
        response["success"] = true;
        response["imported"] = imported;

        String responseStr;
        serializeJson(response, responseStr);
        request->send(200, "application/json", responseStr);
    });

    // Serve static files from LittleFS (MUST be last for API priority)
    // Serwuj pliki statyczne z LittleFS (MUSI być ostatnie dla priorytetu API)
    webServer->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    Serial.println("[Web] Routes configured");
}

/**
 * @brief Handle device polling
 * Obsłuż odpytywanie urządzeń
 */
void handleDevicePolling() {
    if (millis() - lastDevicePoll < SHELLY_POLL_INTERVAL) {
        return;
    }
    lastDevicePoll = millis();

    std::vector<ShellyDevice> devices;
    Storage::loadDevices(devices);

    for (auto& device : devices) {
        if (!device.enabled) {
            continue;
        }

        ShellyMeasurement measurement;
        if (ShellyClient::getMeasurement(device.ip, measurement)) {
            device.lastPower = measurement.power_w;
            device.lastSeen = millis();

            // Evaluate rules with current power / Oceń reguły z aktualną mocą
            // Note: This is simplified - in production you'd get grid export from a meter
            // Uwaga: To jest uproszczone - w produkcji pobierałbyś eksport z licznika
            logicEngine.evaluateRules(measurement.power_w);
        }
    }

    Storage::saveDevices(devices);
}

/**
 * @brief Handle telemetry upload
 * Obsłuż wysyłanie telemetrii
 */
void handleTelemetry() {
    if (millis() - lastTelemetrySync < TELEMETRY_SEND_INTERVAL) {
        return;
    }
    lastTelemetrySync = millis();

    std::vector<ShellyDevice> devices;
    Storage::loadDevices(devices);

    for (const auto& device : devices) {
        JsonDocument telemetry;
        telemetry["device_id"] = device.id;
        telemetry["mac"] = device.mac;
        telemetry["ip"] = device.ip;
        telemetry["last_seen"] = device.lastSeen;

        JsonObject metrics = telemetry["metrics"].to<JsonObject>();
        metrics["power_w"] = device.lastPower;
        metrics["timestamp"] = millis();

        restClient.postDeviceState(device.id, telemetry);
    }
}

/**
 * @brief Handle backend synchronization
 * Obsłuż synchronizację z backendem
 */
void handleBackendSync() {
    if (millis() - lastBackendSync < REST_SYNC_INTERVAL) {
        return;
    }
    lastBackendSync = millis();

    // Sync settings from backend / Synchronizuj ustawienia z backendu
    JsonDocument settings;
    if (restClient.getSettings(settings)) {
        // Update rules if provided / Zaktualizuj reguły jeśli dostarczone
        if (settings.containsKey("rules")) {
            // Process and update rules / Przetwórz i zaktualizuj reguły
            Serial.println("[Main] Settings synced from backend");
        }
    }
}

/**
 * @brief Setup OTA updates
 * Konfiguracja aktualizacji OTA
 */
void setupOTA() {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.setPort(OTA_PORT);

    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        Serial.println("[OTA] Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\n[OTA] Update complete");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    Serial.println("[OTA] Ready");
}

/**
 * @brief Print system information
 * Wyświetl informacje systemowe
 */
void printSystemInfo() {
    Serial.println("\n========== System Information ==========");
    Serial.printf("Chip Model: %s\n", ESP.getChipModel());
    Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
    Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Flash Size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("MAC Address: %s\n", wifiManager.getMACAddress().c_str());
    Serial.printf("WiFi SSID: %s\n", wifiManager.getSSID().c_str());
    Serial.printf("IP Address: %s\n", wifiManager.getIPAddress().c_str());
    Serial.printf("RSSI: %d dBm\n", wifiManager.getRSSI());
    Serial.println("========================================\n");
}

/**
 * @brief Restart web server (called after WiFi state change)
 * Restartuj serwer WWW (wywoływane po zmianie stanu WiFi)
 */
void restartWebServer() {
    Serial.println("[Main] Restarting web server...");

    if (webServer) {
        // End current server
        webServer->end();
        Serial.println("[Main] Web server stopped");

        delay(100); // Small delay for cleanup

        // Recreate server on new interface
        delete webServer;
        webServer = new AsyncWebServer(WEB_SERVER_PORT);

        // Setup routes again
        setupWebRoutes();

        // Start server
        webServer->begin();
        Serial.printf("[Main] Web server restarted on IP: %s\n",
                     wifiManager.getIPAddress().c_str());
    }
}
