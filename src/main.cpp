/**
 * @file main.cpp
 * @brief Pistachio Smart Controller - Main Application
 * @version 2.0.0
 * @author MrBorkim
 *
 * This is a complete ESP32-based smart energy management system that:
 * 1. Monitors energy flow using Shelly EM or PZEM-004T meters
 * 2. Controls Shelly devices based on sophisticated automation rules
 * 3. Optimizes solar energy utilization
 * 4. Prevents unnecessary grid import/export
 * 5. Provides modern web interface for monitoring and control
 *
 * Key Components:
 * - EnergyMeterManager: Reads power data from energy meters
 * - RuleEngine: Evaluates automation rules and controls devices
 * - Web Interface: Modern UI for configuration and monitoring
 * - Discovery: Auto-discovers Shelly devices on network
 *
 * Example Rules:
 * - Use Solar Surplus: IF grid_export > 1500W FOR 60s THEN turn_on(boiler)
 * - Prevent Import: IF grid_import > 500W FOR 30s THEN turn_off(heating)
 * - Time Schedule: IF time = 22:00 THEN turn_off(all_devices)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

// Project includes
#include "config.h"
#include "secrets.h"  // Create from secrets_template.h
#include "EnergyMeterManager.h"
#include "RuleEngine.h"

// ============================================================================
// Global Objects
// ============================================================================
AsyncWebServer server(WEB_SERVER_PORT);
EnergyMeterManager energyMeter;
RuleEngine ruleEngine;

// ============================================================================
// State Variables
// ============================================================================
unsigned long lastEnergyRead = 0;
unsigned long lastRuleEvaluation = 0;
unsigned long lastDiscovery = 0;
bool systemReady = false;
String deviceMAC = "";

// ============================================================================
// Function Prototypes
// ============================================================================
void setupWiFi();
void setupFileSystem();
void setupWebServer();
void setupMDNS();
void handleEnergyMeter();
void handleRuleEngine();
void printSystemInfo();

// ============================================================================
// Setup - Runs once at startup
// ============================================================================
void setup() {
    Serial.begin(DEBUG_SERIAL_BAUD);
    delay(1000);

    // Print banner
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  🥜 Pistachio Smart Controller");
    Serial.println("  ESP32 Energy Management System");
    Serial.printf("  Version: %s\n", FIRMWARE_VERSION);
    Serial.printf("  Build: %s %s\n", BUILD_DATE, BUILD_TIME);
    Serial.println("========================================\n");

    // Initialize status LED
    if (STATUS_LED_ENABLE) {
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, LED_ACTIVE_LOW ? HIGH : LOW);  // OFF
    }

    // Initialize filesystem
    setupFileSystem();

    // Initialize WiFi
    setupWiFi();

    // Get MAC address
    deviceMAC = WiFi.macAddress();
    LOGF("Device MAC: %s", deviceMAC.c_str());

    // Initialize Energy Meter
    LOG("Initializing Energy Meter...");
    if (!energyMeter.begin()) {
        LOGE("Failed to initialize energy meter");
    } else {
        LOG("Energy Meter initialized");
        LOG_METER("Meter type: " + energyMeter.getMeterTypeString());
    }

    // Initialize Rule Engine
    LOG("Initializing Rule Engine...");
    if (!ruleEngine.begin()) {
        LOGE("Failed to initialize rule engine");
    } else {
        LOG("Rule Engine initialized");
        LOGF_RULE("Loaded %d rules", ruleEngine.getRuleCount());
    }

    // Setup mDNS
    setupMDNS();

    // Setup web server
    setupWebServer();

    // Print system info
    printSystemInfo();

    // Turn LED ON to indicate ready
    if (STATUS_LED_ENABLE) {
        digitalWrite(LED_PIN, LED_ACTIVE_LOW ? LOW : HIGH);  // ON
    }

    systemReady = true;
    LOG("\n✅ System Ready!\n");
}

// ============================================================================
// Loop - Runs continuously
// ============================================================================
void loop() {
    // Energy Meter Reading
    handleEnergyMeter();

    // Rule Engine Evaluation
    handleRuleEngine();

    // Small delay to prevent watchdog
    delay(LOOP_DELAY);
}

// ============================================================================
// WiFi Setup
// ============================================================================
void setupWiFi() {
    LOG("Connecting to WiFi...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_CONNECT_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        LOGF("✅ Connected to WiFi: %s", DEFAULT_WIFI_SSID);
        LOGF("IP Address: %s", WiFi.localIP().toString().c_str());
        LOGF("Signal Strength: %d dBm", WiFi.RSSI());
    } else {
        Serial.println();
        LOGW("Failed to connect to WiFi");
        LOGW("Starting Access Point mode...");

        // Start AP mode
        String apSSID = String(AP_SSID_PREFIX) + "-" + WiFi.macAddress().substring(12);
        WiFi.softAP(apSSID.c_str(), AP_PASSWORD);

        LOGF("AP Started: %s", apSSID.c_str());
        LOGF("AP IP: %s", WiFi.softAPIP().toString().c_str());
        LOGF("AP Password: %s", AP_PASSWORD);
    }
}

// ============================================================================
// Filesystem Setup
// ============================================================================
void setupFileSystem() {
    LOG("Initializing LittleFS...");

    if (!LittleFS.begin(FS_FORMAT_ON_FAIL)) {
        LOGE("LittleFS mount failed!");
        return;
    }

    LOG("LittleFS mounted successfully");

    // Print filesystem info
    size_t totalBytes = LittleFS.totalBytes();
    size_t usedBytes = LittleFS.usedBytes();
    LOGF("Storage: %d KB total, %d KB used, %d KB free",
         totalBytes / 1024, usedBytes / 1024, (totalBytes - usedBytes) / 1024);
}

// ============================================================================
// mDNS Setup
// ============================================================================
void setupMDNS() {
    if (MDNS.begin(MDNS_HOSTNAME)) {
        LOG("mDNS responder started");
        LOGF("Hostname: %s.local", MDNS_HOSTNAME);

        MDNS.addService("http", "tcp", WEB_SERVER_PORT);
        MDNS.addService("pistachio", "tcp", WEB_SERVER_PORT);
    } else {
        LOGW("mDNS responder failed to start");
    }
}

// ============================================================================
// Web Server Setup
// ============================================================================
void setupWebServer() {
    LOG("Setting up web server...");

    // Serve static files from LittleFS
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    // API: System Status
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument doc;
        doc["version"] = FIRMWARE_VERSION;
        doc["uptime"] = millis() / 1000;
        doc["wifi"]["connected"] = WiFi.status() == WL_CONNECTED;
        doc["wifi"]["ssid"] = WiFi.SSID();
        doc["wifi"]["ip"] = WiFi.localIP().toString();
        doc["wifi"]["rssi"] = WiFi.RSSI();
        doc["wifi"]["mac"] = WiFi.macAddress();

        size_t total = LittleFS.totalBytes();
        size_t used = LittleFS.usedBytes();
        doc["storage"]["total"] = total;
        doc["storage"]["used"] = used;
        doc["storage"]["free"] = total - used;

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: Current Energy Data
    server.on("/api/energy/current", HTTP_GET, [](AsyncWebServerRequest *request) {
        EnergyMeasurement measurement = energyMeter.getMeasurement();

        JsonDocument doc;
        doc["timestamp"] = measurement.timestamp;
        doc["grid"]["voltage"] = measurement.voltage;
        doc["grid"]["current"] = measurement.current;
        doc["grid"]["power"] = measurement.power;
        doc["grid"]["energy"] = measurement.energyTotal;

        String flow = "standby";
        if (measurement.flow == GridFlowDirection::EXPORT) flow = "export";
        else if (measurement.flow == GridFlowDirection::IMPORT) flow = "import";
        doc["grid"]["flow"] = flow;

        doc["grid"]["export_power"] = measurement.gridExportPower;
        doc["grid"]["import_power"] = measurement.gridImportPower;

        // Placeholder for solar (would come from second meter or calculation)
        doc["solar"]["power"] = 0;
        doc["solar"]["energy_today"] = 0;

        // Consumption calculation (simplified)
        doc["consumption"]["power"] = abs(measurement.power);
        doc["consumption"]["energy_today"] = measurement.energyTotal;

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: Energy Meter Configuration
    server.on("/api/energy/meter", HTTP_GET, [](AsyncWebServerRequest *request) {
        EnergyMeasurement m = energyMeter.getMeasurement();

        JsonDocument doc;
        doc["voltage"] = m.voltage;
        doc["current"] = m.current;
        doc["power"] = m.power;
        doc["energy"] = m.energyTotal;
        doc["frequency"] = m.frequency;
        doc["pf"] = m.powerFactor;

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: Get Rules
    server.on("/api/rules", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument doc;
        ruleEngine.exportRulesToJson(doc);

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: Discovery Scan
    server.on("/api/discovery/scan", HTTP_POST, [](AsyncWebServerRequest *request) {
        LOG("Discovery scan requested");

        // TODO: Implement actual discovery
        // For now, return mock response
        JsonDocument doc;
        doc["discovered"] = 0;
        doc["message"] = "Discovery not yet implemented in this example";

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // 404 Handler
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });

    server.begin();
    LOG("Web server started");
    LOGF("Access web interface at: http://%s or http://%s.local",
         WiFi.localIP().toString().c_str(), MDNS_HOSTNAME);
}

// ============================================================================
// Energy Meter Handler
// ============================================================================
void handleEnergyMeter() {
    if (millis() - lastEnergyRead < ENERGY_METER_POLL_INTERVAL) {
        return;
    }
    lastEnergyRead = millis();

    if (energyMeter.readMeasurement()) {
        EnergyMeasurement m = energyMeter.getMeasurement();

        // Log every 10 readings (10 seconds at 1s interval)
        static int readingCount = 0;
        if (++readingCount >= 10) {
            readingCount = 0;
            LOGF_METER("Power: %.1fW | Voltage: %.1fV | Current: %.2fA | Flow: %s",
                       m.power, m.voltage, m.current,
                       m.flow == GridFlowDirection::EXPORT ? "EXPORT" :
                       m.flow == GridFlowDirection::IMPORT ? "IMPORT" : "STANDBY");

            if (m.flow == GridFlowDirection::EXPORT) {
                LOGF_METER("⬆️ Exporting %.1fW to grid", m.gridExportPower);
            } else if (m.flow == GridFlowDirection::IMPORT) {
                LOGF_METER("⬇️ Importing %.1fW from grid", m.gridImportPower);
            }
        }
    }
}

// ============================================================================
// Rule Engine Handler
// ============================================================================
void handleRuleEngine() {
    if (!systemReady) return;

    if (millis() - lastRuleEvaluation < RULE_EVALUATION_INTERVAL) {
        return;
    }
    lastRuleEvaluation = millis();

    // Get current energy measurement
    EnergyMeasurement measurement = energyMeter.getMeasurement();

    if (!measurement.valid) {
        return;  // Skip if measurement is invalid
    }

    // Evaluate all rules
    ruleEngine.evaluateAllRules(measurement);
}

// ============================================================================
// System Info
// ============================================================================
void printSystemInfo() {
    Serial.println("\n========== System Information ==========");
    Serial.printf("Chip Model: %s\n", ESP.getChipModel());
    Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
    Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Flash Size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("SDK Version: %s\n", ESP.getSdkVersion());
    Serial.println("========================================\n");
}
