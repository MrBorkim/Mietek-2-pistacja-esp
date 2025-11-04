/**
 * @file secrets_template.h
 * @brief Template for sensitive credentials - RENAME TO secrets.h and fill in values
 *
 * IMPORTANT SECURITY NOTICE / WAŻNA UWAGA BEZPIECZEŃSTWA:
 * 1. Copy this file to secrets.h
 * 2. Fill in your actual credentials
 * 3. Add secrets.h to .gitignore to prevent committing credentials
 * 4. Never commit the secrets.h file with real credentials to version control
 *
 * 1. Skopiuj ten plik do secrets.h
 * 2. Wypełnij rzeczywiste dane uwierzytelniające
 * 3. Dodaj secrets.h do .gitignore aby zapobiec commitowaniu danych
 * 4. Nigdy nie commituj pliku secrets.h z prawdziwymi danymi
 */

#ifndef SECRETS_H
#define SECRETS_H

// ============================================================================
// WiFi Credentials / Dane uwierzytelniające WiFi
// ============================================================================
// These are default values loaded if no configuration exists in LittleFS
// Te wartości domyślne są ładowane gdy brak konfiguracji w LittleFS

#define DEFAULT_WIFI_SSID "YOUR_WIFI_SSID"          // Your WiFi network name / Nazwa sieci WiFi
#define DEFAULT_WIFI_PASSWORD "YOUR_WIFI_PASSWORD"  // Your WiFi password / Hasło WiFi

// ============================================================================
// REST Backend Credentials / Dane backendu REST
// ============================================================================
#define REST_API_BASE_URL "http://192.168.1.100:5000/api"  // Backend URL
#define REST_API_USERNAME "admin"                           // API username / Nazwa użytkownika
#define REST_API_PASSWORD "your-secure-password"            // API password / Hasło API
#define REST_API_TOKEN ""                                   // Optional API token / Opcjonalny token

// ============================================================================
// HTTPS Configuration (if using secure connection)
// Konfiguracja HTTPS (jeśli używasz bezpiecznego połączenia)
// ============================================================================
#define REST_API_USE_HTTPS false                            // Set to true for HTTPS / Ustaw na true dla HTTPS

// Example certificate (replace with your actual certificate)
// Przykładowy certyfikat (zamień na swój prawdziwy)
const char* REST_API_ROOT_CA PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
YOUR_CERTIFICATE_HERE
-----END CERTIFICATE-----
)EOF";

// ============================================================================
// OTA Update Credentials / Dane aktualizacji OTA
// ============================================================================
#define OTA_UPDATE_PASSWORD "your-ota-password"             // OTA update password / Hasło OTA

// ============================================================================
// Shelly Device Credentials (if required)
// Dane urządzeń Shelly (jeśli wymagane)
// ============================================================================
// Some Shelly devices may require authentication
// Niektóre urządzenia Shelly mogą wymagać autoryzacji

#define SHELLY_USERNAME ""                                  // Usually not needed / Zwykle niepotrzebne
#define SHELLY_PASSWORD ""                                  // Usually not needed / Zwykle niepotrzebne

// ============================================================================
// Admin Web Interface Credentials
// Dane interfejsu WWW administratora
// ============================================================================
#define WEB_ADMIN_USERNAME "admin"                          // Web interface username
#define WEB_ADMIN_PASSWORD "pistachio-admin"                // Web interface password

// ============================================================================
// Encryption Key (for local storage encryption)
// Klucz szyfrowania (dla szyfrowania lokalnej pamięci)
// ============================================================================
// Simple XOR key for basic obfuscation (NOT secure encryption!)
// Prosty klucz XOR dla podstawowej obfuskacji (NIE jest to bezpieczne szyfrowanie!)
#define ENCRYPTION_KEY "PistachioKey2025"                   // Change this / Zmień to

// ============================================================================
// SECURITY WARNING / OSTRZEŻENIE BEZPIECZEŃSTWA
// ============================================================================
/*
 * This file stores credentials in PLAINTEXT or with WEAK encryption.
 * This is acceptable for:
 *   - Development and testing
 *   - Home networks with trusted users
 *   - Non-critical applications
 *
 * For production environments, consider:
 *   - Using secure element (ATECC608A)
 *   - Implementing proper key derivation (PBKDF2)
 *   - Using encrypted storage with hardware keys
 *   - Certificate-based authentication instead of passwords
 *
 * Ten plik przechowuje dane w POSTACI JAWNEJ lub ze SŁABYM szyfrowaniem.
 * Jest to akceptowalne dla:
 *   - Rozwoju i testów
 *   - Sieci domowych z zaufanymi użytkownikami
 *   - Aplikacji niekrytycznych
 *
 * W środowiskach produkcyjnych rozważ:
 *   - Użycie bezpiecznego elementu (ATECC608A)
 *   - Implementację właściwego wyprowadzania klucza (PBKDF2)
 *   - Szyfrowaną pamięć z kluczami sprzętowymi
 *   - Autoryzację opartą na certyfikatach zamiast haseł
 */

#endif // SECRETS_H
