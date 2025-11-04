/**
 * @file Storage.h
 * @brief LittleFS storage management for persistent configuration
 *
 * Zarządzanie pamięcią LittleFS dla trwałej konfiguracji
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <vector>
#include "config.h"

/**
 * @brief Structure for WiFi configuration / Struktura konfiguracji WiFi
 */
struct WiFiConfig {
    String ssid;
    String password;
    bool configured;
};

/**
 * @brief Structure for Shelly device / Struktura urządzenia Shelly
 */
struct ShellyDevice {
    String id;              // Unique device ID / Unikalny ID urządzenia
    String name;            // Human-readable name / Nazwa czytelna
    String mac;             // MAC address / Adres MAC
    String ip;              // Current IP address / Aktualny adres IP
    bool enabled;           // Device enabled / Urządzenie włączone
    unsigned long lastSeen; // Last seen timestamp / Czas ostatniego wykrycia
    float lastPower;        // Last power reading (W) / Ostatni pomiar mocy
};

/**
 * @brief Structure for logic rule / Struktura reguły logicznej
 */
struct LogicRule {
    String id;              // Rule ID / ID reguły
    String deviceId;        // Target device ID / ID urządzenia docelowego
    float threshold;        // Power threshold (W) / Próg mocy
    float hysteresis;       // Hysteresis value / Wartość histerezy
    unsigned long delayOn;  // Delay before turn on (ms) / Opóźnienie przed włączeniem
    unsigned long delayOff; // Delay before turn off (ms) / Opóźnienie przed wyłączeniem
    int priority;           // Priority (higher = more important) / Priorytet
    bool enabled;           // Rule enabled / Reguła włączona
    String condition;       // Condition type: "export_above", "export_below", etc.
};

/**
 * @brief Structure for REST backend config / Struktura konfiguracji backendu REST
 */
struct BackendConfig {
    String baseUrl;
    String username;
    String password;
    String token;
    bool useHttps;
    unsigned long syncInterval;
};

/**
 * @class Storage
 * @brief Manages persistent storage using LittleFS
 *
 * Zarządza trwałą pamięcią używając LittleFS
 */
class Storage {
public:
    /**
     * @brief Initialize storage system / Inicjalizacja systemu pamięci
     * @return true if successful / prawda jeśli sukces
     */
    static bool begin();

    /**
     * @brief Format storage (WARNING: erases all data) / Formatuj pamięć (UWAGA: usuwa wszystkie dane)
     * @return true if successful / prawda jeśli sukces
     */
    static bool format();

    // ========================================================================
    // WiFi Configuration / Konfiguracja WiFi
    // ========================================================================

    /**
     * @brief Load WiFi configuration / Wczytaj konfigurację WiFi
     * @param config Output configuration / Wyjściowa konfiguracja
     * @return true if loaded successfully / prawda jeśli wczytano pomyślnie
     */
    static bool loadWiFiConfig(WiFiConfig& config);

    /**
     * @brief Save WiFi configuration / Zapisz konfigurację WiFi
     * @param config Configuration to save / Konfiguracja do zapisu
     * @return true if saved successfully / prawda jeśli zapisano pomyślnie
     */
    static bool saveWiFiConfig(const WiFiConfig& config);

    // ========================================================================
    // Device Management / Zarządzanie urządzeniami
    // ========================================================================

    /**
     * @brief Load all Shelly devices / Wczytaj wszystkie urządzenia Shelly
     * @param devices Output vector of devices / Wektor urządzeń wyjściowy
     * @return true if loaded successfully / prawda jeśli wczytano pomyślnie
     */
    static bool loadDevices(std::vector<ShellyDevice>& devices);

    /**
     * @brief Save all Shelly devices / Zapisz wszystkie urządzenia Shelly
     * @param devices Vector of devices to save / Wektor urządzeń do zapisu
     * @return true if saved successfully / prawda jeśli zapisano pomyślnie
     */
    static bool saveDevices(const std::vector<ShellyDevice>& devices);

    /**
     * @brief Update single device / Aktualizuj pojedyncze urządzenie
     * @param device Device to update / Urządzenie do aktualizacji
     * @return true if updated successfully / prawda jeśli zaktualizowano
     */
    static bool updateDevice(const ShellyDevice& device);

    /**
     * @brief Find device by MAC address / Znajdź urządzenie po adresie MAC
     * @param mac MAC address / Adres MAC
     * @param device Output device / Urządzenie wyjściowe
     * @return true if found / prawda jeśli znaleziono
     */
    static bool findDeviceByMAC(const String& mac, ShellyDevice& device);

    // ========================================================================
    // Rules Management / Zarządzanie regułami
    // ========================================================================

    /**
     * @brief Load all logic rules / Wczytaj wszystkie reguły logiczne
     * @param rules Output vector of rules / Wektor reguł wyjściowy
     * @return true if loaded successfully / prawda jeśli wczytano pomyślnie
     */
    static bool loadRules(std::vector<LogicRule>& rules);

    /**
     * @brief Save all logic rules / Zapisz wszystkie reguły logiczne
     * @param rules Vector of rules to save / Wektor reguł do zapisu
     * @return true if saved successfully / prawda jeśli zapisano pomyślnie
     */
    static bool saveRules(const std::vector<LogicRule>& rules);

    // ========================================================================
    // Backend Configuration / Konfiguracja backendu
    // ========================================================================

    /**
     * @brief Load backend configuration / Wczytaj konfigurację backendu
     * @param config Output configuration / Wyjściowa konfiguracja
     * @return true if loaded successfully / prawda jeśli wczytano pomyślnie
     */
    static bool loadBackendConfig(BackendConfig& config);

    /**
     * @brief Save backend configuration / Zapisz konfigurację backendu
     * @param config Configuration to save / Konfiguracja do zapisu
     * @return true if saved successfully / prawda jeśli zapisano pomyślnie
     */
    static bool saveBackendConfig(const BackendConfig& config);

    // ========================================================================
    // State Management / Zarządzanie stanem
    // ========================================================================

    /**
     * @brief Save current state before restart / Zapisz aktualny stan przed restartem
     * @param state JSON document with state / Dokument JSON ze stanem
     * @return true if saved successfully / prawda jeśli zapisano pomyślnie
     */
    static bool saveState(const JsonDocument& state);

    /**
     * @brief Load saved state / Wczytaj zapisany stan
     * @param state Output JSON document / Wyjściowy dokument JSON
     * @return true if loaded successfully / prawda jeśli wczytano pomyślnie
     */
    static bool loadState(JsonDocument& state);

    // ========================================================================
    // Utility Functions / Funkcje narzędziowe
    // ========================================================================

    /**
     * @brief Check if file exists / Sprawdź czy plik istnieje
     * @param path File path / Ścieżka pliku
     * @return true if exists / prawda jeśli istnieje
     */
    static bool fileExists(const String& path);

    /**
     * @brief Get file size / Pobierz rozmiar pliku
     * @param path File path / Ścieżka pliku
     * @return File size in bytes or -1 if error / Rozmiar w bajtach lub -1 przy błędzie
     */
    static long getFileSize(const String& path);

    /**
     * @brief List all files / Wypisz wszystkie pliki
     */
    static void listFiles();

    /**
     * @brief Get storage info / Pobierz informacje o pamięci
     * @param totalBytes Output total bytes / Całkowita liczba bajtów
     * @param usedBytes Output used bytes / Użyte bajty
     */
    static void getStorageInfo(size_t& totalBytes, size_t& usedBytes);

private:
    static bool initialized;

    /**
     * @brief Read JSON file / Wczytaj plik JSON
     * @param path File path / Ścieżka pliku
     * @param doc Output JSON document / Wyjściowy dokument JSON
     * @return true if successful / prawda jeśli sukces
     */
    static bool readJsonFile(const String& path, JsonDocument& doc);

    /**
     * @brief Write JSON file / Zapisz plik JSON
     * @param path File path / Ścieżka pliku
     * @param doc JSON document to write / Dokument JSON do zapisu
     * @return true if successful / prawda jeśli sukces
     */
    static bool writeJsonFile(const String& path, const JsonDocument& doc);

    /**
     * @brief Simple XOR encryption (basic obfuscation) / Proste szyfrowanie XOR
     * @param data Data to encrypt/decrypt / Dane do szyfrowania/deszyfrowania
     * @param key Encryption key / Klucz szyfrowania
     * @return Encrypted/decrypted string / Zaszyfrowany/odszyfrowany ciąg
     */
    static String xorEncrypt(const String& data, const String& key);
};

#endif // STORAGE_H
