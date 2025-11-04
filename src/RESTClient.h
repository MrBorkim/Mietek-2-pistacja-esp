/**
 * @file RESTClient.h
 * @brief REST API client for backend communication
 *
 * Klient REST API dla komunikacji z backendem
 */

#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "Storage.h"

/**
 * @class RESTClient
 * @brief Handles HTTP/HTTPS communication with backend server
 *
 * Obsługuje komunikację HTTP/HTTPS z serwerem backendu
 */
class RESTClient {
public:
    /**
     * @brief Initialize REST client / Inicjalizacja klienta REST
     * @param config Backend configuration / Konfiguracja backendu
     * @return true if successful / prawda jeśli sukces
     */
    bool begin(const BackendConfig& config);

    /**
     * @brief GET request to fetch device configuration
     * @param deviceId Device ID / ID urządzenia
     * @param response Output JSON response / Odpowiedź JSON
     * @return true if successful / prawda jeśli sukces
     */
    bool getDeviceConfig(const String& deviceId, JsonDocument& response);

    /**
     * @brief POST device state/telemetry
     * @param deviceId Device ID / ID urządzenia
     * @param state State data / Dane stanu
     * @return true if successful / prawda jeśli sukces
     */
    bool postDeviceState(const String& deviceId, const JsonDocument& state);

    /**
     * @brief GET global settings and rules
     * @param settings Output settings / Ustawienia wyjściowe
     * @return true if successful / prawda jeśli sukces
     */
    bool getSettings(JsonDocument& settings);

    /**
     * @brief POST updated settings
     * @param settings Settings to update / Ustawienia do aktualizacji
     * @return true if successful / prawda jeśli sukces
     */
    bool postSettings(const JsonDocument& settings);

    /**
     * @brief Check if backend is reachable / Sprawdź czy backend jest osiągalny
     * @return true if reachable / prawda jeśli osiągalny
     */
    bool ping();

    /**
     * @brief Set backend configuration / Ustaw konfigurację backendu
     * @param config Backend configuration / Konfiguracja backendu
     */
    void setConfig(const BackendConfig& config);

private:
    BackendConfig config;
    HTTPClient http;

    /**
     * @brief Perform HTTP GET request / Wykonaj żądanie HTTP GET
     * @param endpoint API endpoint / Endpoint API
     * @param response Output response / Odpowiedź wyjściowa
     * @return HTTP response code / Kod odpowiedzi HTTP
     */
    int httpGet(const String& endpoint, String& response);

    /**
     * @brief Perform HTTP POST request / Wykonaj żądanie HTTP POST
     * @param endpoint API endpoint / Endpoint API
     * @param payload JSON payload / Ładunek JSON
     * @param response Output response / Odpowiedź wyjściowa
     * @return HTTP response code / Kod odpowiedzi HTTP
     */
    int httpPost(const String& endpoint, const String& payload, String& response);

    /**
     * @brief Add authentication headers / Dodaj nagłówki autoryzacji
     */
    void addAuthHeaders();

    /**
     * @brief Build full URL / Zbuduj pełny URL
     * @param endpoint API endpoint / Endpoint API
     * @return Full URL / Pełny URL
     */
    String buildURL(const String& endpoint);
};

#endif // RESTCLIENT_H
