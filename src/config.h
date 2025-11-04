/**
 * @file config.h
 * @brief Global configuration and constants for Pistachio Smart Controller
 *
 * Główny plik konfiguracyjny projektu - zawiera wszystkie stałe i ustawienia
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// WiFi Configuration / Konfiguracja WiFi
// ============================================================================
#define AP_SSID_PREFIX "Pistachio-Config-"  // Prefix for AP name / Prefiks nazwy AP
#define AP_PASSWORD "pistachio123"          // AP password (min 8 chars) / Hasło AP
#define WIFI_CONNECT_TIMEOUT 30000          // WiFi connection timeout (ms) / Timeout połączenia WiFi
#define WIFI_RETRY_DELAY 5000               // Delay between WiFi retries (ms) / Opóźnienie między próbami
#define WIFI_MAX_RETRIES 5                  // Max WiFi connection retries / Maks. liczba prób połączenia
#define AP_FALLBACK_TIMEOUT 60000           // Time before falling back to AP (ms) / Czas przed powrotem do AP

// ============================================================================
// Web Server Configuration / Konfiguracja serwera WWW
// ============================================================================
#define WEB_SERVER_PORT 80                  // HTTP server port / Port serwera HTTP
#define WEB_SOCKET_PORT 81                  // WebSocket port (if needed) / Port WebSocket

// ============================================================================
// REST Backend Configuration / Konfiguracja backendu REST
// ============================================================================
#define REST_TIMEOUT 10000                  // REST API timeout (ms) / Timeout REST API
#define REST_RETRY_COUNT 3                  // Number of retries / Liczba powtórzeń
#define REST_RETRY_DELAY 2000               // Delay between retries (ms) / Opóźnienie między próbami
#define REST_SYNC_INTERVAL 60000            // Backend sync interval (ms) / Interwał synchronizacji
#define REST_USE_HTTPS false                // Use HTTPS (requires cert) / Użyj HTTPS

// ============================================================================
// Shelly Device Configuration / Konfiguracja urządzeń Shelly
// ============================================================================
#define SHELLY_HTTP_TIMEOUT 5000            // Shelly HTTP timeout (ms) / Timeout HTTP Shelly
#define SHELLY_POLL_INTERVAL 5000           // Device polling interval (ms) / Interwał odpytywania
#define SHELLY_MAX_DEVICES 10               // Maximum Shelly devices / Maksymalna liczba urządzeń
#define SHELLY_RETRY_COUNT 3                // Number of retries / Liczba powtórzeń
#define SHELLY_GEN4_API_PATH "/rpc/"        // Shelly Gen4 API path / Ścieżka API Gen4

// ============================================================================
// Discovery Configuration / Konfiguracja odkrywania urządzeń
// ============================================================================
#define DISCOVERY_INTERVAL 300000           // Discovery scan interval (5 min) / Interwał skanowania
#define DISCOVERY_QUICK_SCAN true           // Use quick scan first / Najpierw szybkie skan
#define MDNS_ENABLE true                    // Enable mDNS discovery / Włącz mDNS
#define UPNP_ENABLE true                    // Enable UPnP/SSDP discovery / Włącz UPnP
#define ARP_SCAN_ENABLE true                // Enable ARP scanning / Włącz skanowanie ARP
#define NETWORK_SCAN_RANGE 254              // IP range to scan (1-254) / Zakres IP do skanowania
#define DISCOVERY_TIMEOUT 30000             // Discovery timeout (ms) / Timeout odkrywania

// ============================================================================
// Logic Engine Configuration / Konfiguracja silnika logiki
// ============================================================================
#define LOGIC_UPDATE_INTERVAL 2000          // Logic evaluation interval (ms) / Interwał oceny logiki
#define RULE_MAX_COUNT 20                   // Maximum rules / Maksymalna liczba reguł
#define HYSTERESIS_DEFAULT 0.1              // Default hysteresis (10%) / Domyślna histereza
#define DELAY_BEFORE_ON 5000                // Delay before turning on (ms) / Opóźnienie przed włączeniem
#define DELAY_BEFORE_OFF 30000              // Delay before turning off (ms) / Opóźnienie przed wyłączeniem
#define POWER_EXPORT_THRESHOLD 1000.0       // Default export threshold (W) / Domyślny próg eksportu

// ============================================================================
// Storage Configuration / Konfiguracja pamięci
// ============================================================================
#define STORAGE_FORMAT_ON_FAIL true         // Format if mount fails / Formatuj przy błędzie montowania
#define CONFIG_FILE_PATH "/config.json"     // Main config file / Główny plik konfiguracji
#define DEVICES_FILE_PATH "/devices.json"   // Devices list file / Plik listy urządzeń
#define RULES_FILE_PATH "/rules.json"       // Rules file / Plik reguł
#define STATE_FILE_PATH "/state.json"       // State file / Plik stanu
#define MAX_FILE_SIZE 32768                 // Max config file size (32KB) / Maks. rozmiar pliku

// ============================================================================
// Telemetry Configuration / Konfiguracja telemetrii
// ============================================================================
#define TELEMETRY_SEND_INTERVAL 30000       // Telemetry upload interval (30s) / Interwał wysyłania
#define TELEMETRY_BUFFER_SIZE 10            // Number of readings to buffer / Bufor odczytów
#define TELEMETRY_BATCH_SEND true           // Send in batches / Wysyłaj grupowo

// ============================================================================
// Watchdog Configuration / Konfiguracja watchdoga
// ============================================================================
#define WATCHDOG_TIMEOUT 30                 // Watchdog timeout (seconds) / Timeout watchdoga (sekundy)
#define TASK_WATCHDOG_ENABLE true           // Enable task watchdog / Włącz watchdog zadań

// ============================================================================
// OTA Update Configuration / Konfiguracja aktualizacji OTA
// ============================================================================
#define OTA_ENABLE true                     // Enable OTA updates / Włącz aktualizacje OTA
#define OTA_PORT 3232                       // OTA port / Port OTA
#define OTA_PASSWORD "pistachio-ota"        // OTA password / Hasło OTA
#define OTA_HOSTNAME "pistachio"            // OTA hostname / Nazwa hosta OTA

// ============================================================================
// Debug Configuration / Konfiguracja debugowania
// ============================================================================
#define DEBUG_ENABLE true                   // Enable debug output / Włącz debug
#define DEBUG_VERBOSE false                 // Verbose debug / Szczegółowy debug
#define DEBUG_SERIAL_BAUD 115200            // Serial baud rate / Prędkość portu szeregowego

// ============================================================================
// Security Configuration / Konfiguracja bezpieczeństwa
// ============================================================================
#define SECURITY_ENCRYPT_PASSWORDS false    // Encrypt stored passwords (XOR) / Szyfruj hasła
#define SECURITY_MIN_PASSWORD_LENGTH 8      // Minimum password length / Min. długość hasła
#define SECURITY_REQUIRE_AUTH true          // Require authentication / Wymagaj autoryzacji
#define ENCRYPTION_KEY "PistachioKey2025"   // XOR encryption key / Klucz szyfrowania XOR

// ============================================================================
// Hardware Configuration / Konfiguracja sprzętowa
// ============================================================================
#define LED_PIN 2                           // Built-in LED pin / Pin LED wbudowanego
#define LED_ACTIVE_LOW true                 // LED active low / LED aktywny w stanie niskim
#define STATUS_LED_ENABLE true              // Enable status LED / Włącz LED statusu

// ============================================================================
// Timing Constants / Stałe czasowe
// ============================================================================
#define LOOP_DELAY 100                      // Main loop delay (ms) / Opóźnienie głównej pętli
#define TASK_STACK_SIZE 8192                // FreeRTOS task stack size / Rozmiar stosu zadań

// ============================================================================
// Version Information / Informacje o wersji
// ============================================================================
#define FIRMWARE_VERSION "1.0.0"            // Firmware version / Wersja firmware
#define HARDWARE_VERSION "ESP32-DevKit"     // Hardware version / Wersja sprzętowa
#define BUILD_DATE __DATE__                 // Build date / Data kompilacji
#define BUILD_TIME __TIME__                 // Build time / Czas kompilacji

#endif // CONFIG_H
