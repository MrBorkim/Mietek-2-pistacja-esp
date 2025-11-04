/**
 * @file WiFiManager.cpp
 * @brief Implementation of WiFi and AP management
 *
 * Implementacja zarządzania WiFi i AP
 */

#include "WiFiManager.h"

WiFiManager::WiFiManager()
    : state(WiFiManagerState::DISCONNECTED),
      webServer(nullptr),
      dnsServer(nullptr),
      lastConnectAttempt(0),
      retryCount(0),
      apMode(false) {
}

bool WiFiManager::begin() {
    Serial.println("[WiFi] Initializing WiFi Manager...");

    // Set WiFi mode / Ustaw tryb WiFi
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);

    // Generate unique AP SSID / Generuj unikalny SSID AP
    apSSID = generateAPSSID();

    // Setup event handler / Ustaw obsługę zdarzeń
    WiFi.onEvent(onWiFiEvent);

    // Always setup web server / Zawsze konfiguruj serwer WWW
    setupWebServer();

    // Load WiFi configuration / Wczytaj konfigurację WiFi
    if (Storage::loadWiFiConfig(wifiConfig) && wifiConfig.configured) {
        Serial.printf("[WiFi] Loaded configuration: SSID='%s'\n", wifiConfig.ssid.c_str());

        // Try to connect, if fails start AP
        // Spróbuj połączyć, jeśli nie uda się uruchom AP
        if (!connectToWiFi()) {
            Serial.println("[WiFi] Connection failed, starting AP mode for reconfiguration");
            return startAP();
        }
        return true;
    } else {
        Serial.println("[WiFi] No configuration found, starting AP mode");
        return startAP();
    }
}

void WiFiManager::loop() {
    if (apMode && dnsServer) {
        dnsServer->processNextRequest();
    }

    checkConnection();
}

bool WiFiManager::connectToWiFi() {
    return connectToWiFi(wifiConfig.ssid, wifiConfig.password);
}

bool WiFiManager::connectToWiFi(const String& ssid, const String& password) {
    if (ssid.length() == 0) {
        Serial.println("[WiFi] ERROR: Empty SSID");
        return false;
    }

    Serial.printf("[WiFi] Connecting to '%s'...\n", ssid.c_str());
    state = WiFiManagerState::CONNECTING;

    // Stop AP if running / Zatrzymaj AP jeśli działa
    if (apMode) {
        stopAP();
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    // Wait for connection with timeout / Czekaj na połączenie z timeoutem
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED &&
           millis() - startAttempt < WIFI_CONNECT_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        state = WiFiManagerState::CONNECTED;
        retryCount = 0;
        Serial.printf("[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("[WiFi] RSSI: %d dBm\n", WiFi.RSSI());

        // Save configuration if successful / Zapisz konfigurację jeśli sukces
        wifiConfig.ssid = ssid;
        wifiConfig.password = password;
        wifiConfig.configured = true;
        Storage::saveWiFiConfig(wifiConfig);

        return true;
    } else {
        state = WiFiManagerState::FAILED;
        Serial.printf("[WiFi] Connection failed! Status: %d\n", WiFi.status());
        Serial.println("[WiFi] Please reconfigure WiFi in AP mode");

        return false;
    }
}

bool WiFiManager::startAP() {
    Serial.printf("[WiFi] Starting Access Point: %s\n", apSSID.c_str());

    WiFi.mode(WIFI_AP);

    bool success = WiFi.softAP(apSSID.c_str(), AP_PASSWORD);

    if (!success) {
        Serial.println("[WiFi] ERROR: Failed to start AP");
        state = WiFiManagerState::FAILED;
        return false;
    }

    // Configure IP address / Konfiguruj adres IP
    IPAddress ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(ip, gateway, subnet);

    state = WiFiManagerState::AP_MODE;
    apMode = true;

    Serial.printf("[WiFi] AP started - IP: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf("[WiFi] AP SSID: %s\n", apSSID.c_str());
    Serial.printf("[WiFi] AP Password: %s\n", AP_PASSWORD);

    // Start DNS server for captive portal / Uruchom serwer DNS dla captive portal
    if (!dnsServer) {
        dnsServer = new DNSServer();
    }
    dnsServer->start(53, "*", WiFi.softAPIP());

    // Setup web server / Ustaw serwer WWW
    setupWebServer();

    return true;
}

void WiFiManager::stopAP() {
    Serial.println("[WiFi] Stopping Access Point");

    if (dnsServer) {
        dnsServer->stop();
    }

    WiFi.softAPdisconnect(true);
    apMode = false;

    if (state == WiFiManagerState::AP_MODE) {
        state = WiFiManagerState::DISCONNECTED;
    }
}

bool WiFiManager::isConnected() const {
    return (state == WiFiManagerState::CONNECTED && WiFi.status() == WL_CONNECTED);
}

bool WiFiManager::isAPMode() const {
    return apMode;
}

WiFiManagerState WiFiManager::getState() const {
    return state;
}

String WiFiManager::getIPAddress() const {
    if (isConnected()) {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

String WiFiManager::getAPIPAddress() const {
    if (apMode) {
        return WiFi.softAPIP().toString();
    }
    return "0.0.0.0";
}

String WiFiManager::getSSID() const {
    if (isConnected()) {
        return WiFi.SSID();
    } else if (apMode) {
        return apSSID;
    }
    return "";
}

int WiFiManager::getRSSI() const {
    if (isConnected()) {
        return WiFi.RSSI();
    }
    return 0;
}

String WiFiManager::getMACAddress() const {
    return WiFi.macAddress();
}

void WiFiManager::resetConfig() {
    Serial.println("[WiFi] Resetting configuration");

    wifiConfig.ssid = "";
    wifiConfig.password = "";
    wifiConfig.configured = false;
    Storage::saveWiFiConfig(wifiConfig);

    WiFi.disconnect(true);
    retryCount = 0;

    startAP();
}

void WiFiManager::setupWebServer() {
    if (!webServer) {
        webServer = new AsyncWebServer(WEB_SERVER_PORT);
    }

    // Note: Actual web server routes will be set up in main.cpp
    // This allows flexibility to add more routes beyond WiFi configuration
    // Uwaga: Właściwe trasy serwera WWW będą ustawione w main.cpp
    // To pozwala na elastyczne dodawanie tras poza konfiguracją WiFi

    Serial.println("[WiFi] Web server ready for route setup");
}

AsyncWebServer* WiFiManager::getWebServer() {
    return webServer;
}

String WiFiManager::generateAPSSID() {
    // Generate unique SSID using chip ID / Generuj unikalny SSID używając ID chipa
    uint32_t chipId = 0;
    for (int i = 0; i < 17; i = i + 8) {
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }

    char ssid[32];
    snprintf(ssid, sizeof(ssid), "%s%04X", AP_SSID_PREFIX, (uint16_t)(chipId & 0xFFFF));
    return String(ssid);
}

void WiFiManager::onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("[WiFi Event] Station started");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("[WiFi Event] Connected to AP");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.printf("[WiFi Event] Got IP: %s\n", WiFi.localIP().toString().c_str());
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[WiFi Event] Disconnected from AP");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("[WiFi Event] AP started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("[WiFi Event] Client connected to AP");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("[WiFi Event] Client disconnected from AP");
            break;
        default:
            break;
    }
}

void WiFiManager::checkConnection() {
    static unsigned long lastCheck = 0;

    if (millis() - lastCheck < 5000) {
        return; // Check every 5 seconds / Sprawdzaj co 5 sekund
    }
    lastCheck = millis();

    if (!apMode && state == WiFiManagerState::CONNECTED) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WiFi] Connection lost, attempting reconnect...");
            state = WiFiManagerState::DISCONNECTED;
            retryCount++;

            if (retryCount >= WIFI_MAX_RETRIES) {
                Serial.println("[WiFi] Too many failures, starting AP mode");
                startAP();
            } else {
                connectToWiFi();
            }
        }
    }
}
