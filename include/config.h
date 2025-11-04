/**
 * @file config.h
 * @brief Global configuration for Pistachio Smart Controller
 * @version 2.0.0
 * @author MrBorkim
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Firmware Version
// ============================================================================
#define FIRMWARE_VERSION "2.0.0"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

// ============================================================================
// Debug Settings
// ============================================================================
#define DEBUG_ENABLE true
#define DEBUG_VERBOSE false
#define DEBUG_SERIAL_BAUD 115200
#define DEBUG_RULE_ENGINE true
#define DEBUG_ENERGY_METER true

// ============================================================================
// WiFi Configuration
// ============================================================================
#define AP_SSID_PREFIX "Pistachio-Config"
#define AP_PASSWORD "pistachio123"
#define WIFI_CONNECT_TIMEOUT 30000  // 30 seconds
#define WIFI_RECONNECT_INTERVAL 60000  // 1 minute

// ============================================================================
// Web Server
// ============================================================================
#define WEB_SERVER_PORT 80
#define WEBSOCKET_PORT 81
#define ENABLE_WEBSOCKET true

// ============================================================================
// Hardware Pins
// ============================================================================
// Status LED
#define LED_PIN 2
#define LED_ACTIVE_LOW true
#define STATUS_LED_ENABLE true

// PZEM-004T Serial Connection
#define PZEM_RX_PIN 16  // GPIO16 (RX2)
#define PZEM_TX_PIN 17  // GPIO17 (TX2)
#define PZEM_SERIAL Serial2
#define PZEM_BAUD 9600

// Optional: External buttons/sensors
#define BUTTON_PIN -1  // Not used
#define SENSOR_PIN -1  // Not used

// ============================================================================
// Timing Configuration
// ============================================================================
#define LOOP_DELAY 10  // Main loop delay (ms)

// Device polling
#define SHELLY_POLL_INTERVAL 5000  // 5 seconds
#define ENERGY_METER_POLL_INTERVAL 1000  // 1 second (fast for real-time)
#define DISCOVERY_INTERVAL 300000  // 5 minutes

// Telemetry & Sync
#define TELEMETRY_SEND_INTERVAL 60000  // 1 minute
#define REST_SYNC_INTERVAL 300000  // 5 minutes

// Rule engine
#define RULE_EVALUATION_INTERVAL 2000  // 2 seconds
#define RULE_MAX_DELAY_ON 3600  // Max delay ON: 1 hour
#define RULE_MAX_DELAY_OFF 3600  // Max delay OFF: 1 hour

// ============================================================================
// Storage Configuration
// ============================================================================
#define FS_FORMAT_ON_FAIL false
#define CONFIG_FILE_PATH "/config.json"
#define DEVICES_FILE_PATH "/devices.json"
#define RULES_FILE_PATH "/rules.json"
#define METER_CONFIG_PATH "/meter.json"
#define HISTORY_FILE_PATH "/history.json"

// History retention
#define HISTORY_RETENTION_DAYS 7
#define HISTORY_MAX_RECORDS 1000

// ============================================================================
// Energy Meter Configuration
// ============================================================================
// Shelly EM
#define SHELLY_EM_TIMEOUT 5000  // 5 seconds
#define SHELLY_EM_RETRY_COUNT 3

// PZEM-004T
#define PZEM_READ_TIMEOUT 1000  // 1 second
#define PZEM_MAX_ADDRESS 247
#define PZEM_DEFAULT_ADDRESS 0x01

// Energy calculation
#define VOLTAGE_NOMINAL 230.0  // Nominal voltage (V)
#define GRID_FREQUENCY_NOMINAL 50.0  // Nominal frequency (Hz)

// ============================================================================
// Rule Engine Configuration
// ============================================================================
#define MAX_RULES 50
#define MAX_CONDITIONS_PER_RULE 10
#define MAX_ACTIONS_PER_RULE 10
#define DEFAULT_HYSTERESIS 200  // Watts
#define DEFAULT_DELAY_ON 60  // seconds
#define DEFAULT_DELAY_OFF 300  // seconds

// Condition thresholds
#define GRID_EXPORT_MIN_THRESHOLD 100  // Minimum export power (W)
#define GRID_IMPORT_MIN_THRESHOLD 50  // Minimum import power (W)
#define SOLAR_PRODUCTION_MIN 10  // Minimum solar production (W)

// ============================================================================
// Shelly Device Configuration
// ============================================================================
#define MAX_SHELLY_DEVICES 32
#define SHELLY_RPC_TIMEOUT 5000  // 5 seconds
#define SHELLY_DISCOVER_TIMEOUT 10000  // 10 seconds
#define SHELLY_MAX_RETRIES 3

// Shelly Gen2/Gen4 RPC endpoints
#define SHELLY_RPC_SWITCH_SET "/rpc/Switch.Set"
#define SHELLY_RPC_SWITCH_GET_STATUS "/rpc/Switch.GetStatus"
#define SHELLY_RPC_SHELLY_GET_STATUS "/rpc/Shelly.GetStatus"
#define SHELLY_RPC_SHELLY_GET_DEVICE_INFO "/rpc/Shelly.GetDeviceInfo"
#define SHELLY_RPC_EM_GET_STATUS "/rpc/EM.GetStatus"

// mDNS
#define MDNS_HOSTNAME "pistachio-controller"
#define MDNS_SERVICE "_shelly"
#define MDNS_PROTOCOL "_tcp"

// ============================================================================
// OTA Configuration
// ============================================================================
#define OTA_ENABLE true
#define OTA_HOSTNAME "pistachio-ota"
#define OTA_PASSWORD "pistachio-ota"
#define OTA_PORT 3232

// ============================================================================
// Backend REST API Configuration (Optional)
// ============================================================================
#define BACKEND_API_ENABLE false
#define BACKEND_API_URL ""
#define BACKEND_API_KEY ""
#define BACKEND_TIMEOUT 10000  // 10 seconds

// ============================================================================
// Advanced Features
// ============================================================================
// Power flow calculation modes
enum class PowerFlowMode {
    SINGLE_METER_GRID,      // Single meter on grid connection
    SINGLE_METER_SOLAR,     // Single meter on solar production
    DUAL_METER,             // Two meters (grid + solar)
    THREE_PHASE             // Three-phase system (Shelly 3EM)
};

#define DEFAULT_POWER_FLOW_MODE PowerFlowMode::SINGLE_METER_GRID

// Grid export/import detection
enum class GridFlowDirection {
    STANDBY,    // No significant flow
    EXPORT,     // Selling to grid (negative power)
    IMPORT      // Buying from grid (positive power)
};

#define GRID_FLOW_STANDBY_THRESHOLD 50  // ±50W is considered standby

// Energy pricing (for savings calculation)
#define ENERGY_PRICE_KWH 0.80  // PLN per kWh
#define FEED_IN_TARIFF_KWH 0.20  // PLN per kWh for selling

// ============================================================================
// Macros & Utilities
// ============================================================================
// Debug logging
#if DEBUG_ENABLE
    #define LOG(msg) Serial.println(String("[LOG] ") + msg)
    #define LOGF(format, ...) Serial.printf("[LOG] " format "\n", ##__VA_ARGS__)
#else
    #define LOG(msg)
    #define LOGF(format, ...)
#endif

#if DEBUG_VERBOSE
    #define LOGV(msg) Serial.println(String("[VERBOSE] ") + msg)
    #define LOGVF(format, ...) Serial.printf("[VERBOSE] " format "\n", ##__VA_ARGS__)
#else
    #define LOGV(msg)
    #define LOGVF(format, ...)
#endif

// Error logging (always enabled)
#define LOGE(msg) Serial.println(String("[ERROR] ") + msg)
#define LOGEF(format, ...) Serial.printf("[ERROR] " format "\n", ##__VA_ARGS__)

// Warning logging
#define LOGW(msg) Serial.println(String("[WARN] ") + msg)
#define LOGWF(format, ...) Serial.printf("[WARN] " format "\n", ##__VA_ARGS__)

// Module-specific logging
#if DEBUG_RULE_ENGINE
    #define LOG_RULE(msg) Serial.println(String("[RuleEngine] ") + msg)
    #define LOGF_RULE(format, ...) Serial.printf("[RuleEngine] " format "\n", ##__VA_ARGS__)
#else
    #define LOG_RULE(msg)
    #define LOGF_RULE(format, ...)
#endif

#if DEBUG_ENERGY_METER
    #define LOG_METER(msg) Serial.println(String("[EnergyMeter] ") + msg)
    #define LOGF_METER(format, ...) Serial.printf("[EnergyMeter] " format "\n", ##__VA_ARGS__)
#else
    #define LOG_METER(msg)
    #define LOGF_METER(format, ...)
#endif

// ============================================================================
// Safety Limits
// ============================================================================
#define MAX_POWER_RATING 10000  // Max 10kW per device
#define MAX_CURRENT_RATING 50  // Max 50A per device
#define MAX_VOLTAGE_THRESHOLD 250  // Max 250V
#define MIN_VOLTAGE_THRESHOLD 200  // Min 200V

// Anomaly detection
#define ENABLE_ANOMALY_DETECTION true
#define VOLTAGE_ANOMALY_THRESHOLD 10  // ±10V from nominal
#define FREQUENCY_ANOMALY_THRESHOLD 1  // ±1Hz from nominal

#endif // CONFIG_H
