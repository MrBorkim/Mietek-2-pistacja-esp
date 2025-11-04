/**
 * @file WiFiManager.h
 * @brief WiFi connection and Access Point management
 *
 * Zarządzanie połączeniem WiFi i trybem Access Point
 */

#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "Storage.h"

/**
 * @enum WiFiManagerState
 * @brief WiFi manager operational states / Stany operacyjne menedżera WiFi
 */
enum class WiFiManagerState {
    DISCONNECTED,       // Not connected / Niepołączony
    CONNECTING,         // Attempting to connect / Próba połączenia
    CONNECTED,          // Successfully connected / Pomyślnie połączony
    AP_MODE,            // Running as Access Point / Działa jako Access Point
    FAILED              // Connection failed / Połączenie nieudane
};

/**
 * @class WiFiManager
 * @brief Manages WiFi connectivity and AP configuration mode
 *
 * Zarządza łącznością WiFi i trybem konfiguracji AP
 */
class WiFiManager {
public:
    /**
     * @brief Constructor
     */
    WiFiManager();

    /**
     * @brief Initialize WiFi manager / Inicjalizacja menedżera WiFi
     * @return true if successful / prawda jeśli sukces
     */
    bool begin();

    /**
     * @brief Main update loop - call regularly / Główna pętla - wywoływać regularnie
     */
    void loop();

    /**
     * @brief Connect to WiFi using stored credentials / Połącz z WiFi używając zapisanych danych
     * @return true if connection successful / prawda jeśli połączenie udane
     */
    bool connectToWiFi();

    /**
     * @brief Connect to WiFi with specific credentials / Połącz z WiFi podanymi danymi
     * @param ssid WiFi SSID
     * @param password WiFi password / Hasło WiFi
     * @return true if connection successful / prawda jeśli połączenie udane
     */
    bool connectToWiFi(const String& ssid, const String& password);

    /**
     * @brief Start Access Point mode / Uruchom tryb Access Point
     * @return true if successful / prawda jeśli sukces
     */
    bool startAP();

    /**
     * @brief Stop Access Point mode / Zatrzymaj tryb Access Point
     */
    void stopAP();

    /**
     * @brief Check if connected to WiFi / Sprawdź czy połączony z WiFi
     * @return true if connected / prawda jeśli połączony
     */
    bool isConnected() const;

    /**
     * @brief Check if in AP mode / Sprawdź czy w trybie AP
     * @return true if in AP mode / prawda jeśli w trybie AP
     */
    bool isAPMode() const;

    /**
     * @brief Get current state / Pobierz aktualny stan
     * @return Current WiFiManagerState / Aktualny stan
     */
    WiFiManagerState getState() const;

    /**
     * @brief Get current IP address / Pobierz aktualny adres IP
     * @return IP address as String / Adres IP jako String
     */
    String getIPAddress() const;

    /**
     * @brief Get AP IP address / Pobierz adres IP AP
     * @return AP IP address / Adres IP AP
     */
    String getAPIPAddress() const;

    /**
     * @brief Get SSID / Pobierz SSID
     * @return Current SSID / Aktualny SSID
     */
    String getSSID() const;

    /**
     * @brief Get RSSI signal strength / Pobierz siłę sygnału RSSI
     * @return RSSI value in dBm / Wartość RSSI w dBm
     */
    int getRSSI() const;

    /**
     * @brief Get MAC address / Pobierz adres MAC
     * @return MAC address / Adres MAC
     */
    String getMACAddress() const;

    /**
     * @brief Reset WiFi configuration / Resetuj konfigurację WiFi
     */
    void resetConfig();

    /**
     * @brief Setup web server for AP configuration / Ustaw serwer WWW dla konfiguracji AP
     */
    void setupWebServer();

    /**
     * @brief Get web server instance / Pobierz instancję serwera WWW
     * @return Pointer to AsyncWebServer / Wskaźnik do AsyncWebServer
     */
    AsyncWebServer* getWebServer();

private:
    WiFiManagerState state;
    WiFiConfig wifiConfig;
    AsyncWebServer* webServer;
    DNSServer* dnsServer;
    String apSSID;
    unsigned long lastConnectAttempt;
    int retryCount;
    bool apMode;

    /**
     * @brief Generate unique AP SSID / Generuj unikalny SSID AP
     * @return AP SSID
     */
    String generateAPSSID();

    /**
     * @brief Handle WiFi events / Obsługa zdarzeń WiFi
     */
    static void onWiFiEvent(WiFiEvent_t event);

    /**
     * @brief Check connection and handle fallback to AP / Sprawdź połączenie i obsłuż powrót do AP
     */
    void checkConnection();
};

#endif // WIFIMANAGER_H
