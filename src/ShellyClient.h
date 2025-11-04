/**
 * @file ShellyClient.h
 * @brief Shelly Gen4 device control and monitoring
 *
 * Sterowanie i monitorowanie urządzeń Shelly Gen4
 */

#ifndef SHELLYCLIENT_H
#define SHELLYCLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

/**
 * @brief Shelly device measurement data / Dane pomiarowe urządzenia Shelly
 */
struct ShellyMeasurement {
    float power_w;          // Active power in Watts / Moc czynna w watach
    float voltage_v;        // Voltage / Napięcie
    float current_a;        // Current in Amperes / Prąd w amperach
    float energy_wh;        // Energy counter in Wh / Licznik energii w Wh
    float pf;               // Power factor / Współczynnik mocy
    bool valid;             // Measurement validity / Poprawność pomiaru
    unsigned long timestamp; // Measurement time / Czas pomiaru
};

/**
 * @class ShellyClient
 * @brief Controls Shelly Gen4 devices via HTTP API
 *
 * Steruje urządzeniami Shelly Gen4 przez API HTTP
 */
class ShellyClient {
public:
    /**
     * @brief Get device information / Pobierz informacje o urządzeniu
     * @param ip Device IP address / Adres IP urządzenia
     * @param info Output device info JSON / Wyjściowe info JSON
     * @return true if successful / prawda jeśli sukces
     */
    static bool getDeviceInfo(const String& ip, JsonDocument& info);

    /**
     * @brief Get device status / Pobierz status urządzenia
     * @param ip Device IP address / Adres IP urządzenia
     * @param status Output status JSON / Wyjściowy status JSON
     * @return true if successful / prawda jeśli sukces
     */
    static bool getStatus(const String& ip, JsonDocument& status);

    /**
     * @brief Get power measurements / Pobierz pomiary mocy
     * @param ip Device IP address / Adres IP urządzenia
     * @param measurement Output measurement data / Wyjściowe dane pomiarowe
     * @return true if successful / prawda jeśli sukces
     */
    static bool getMeasurement(const String& ip, ShellyMeasurement& measurement);

    /**
     * @brief Turn switch ON / Włącz przełącznik
     * @param ip Device IP address / Adres IP urządzenia
     * @param channel Switch channel (default 0) / Kanał przełącznika
     * @return true if successful / prawda jeśli sukces
     */
    static bool turnOn(const String& ip, int channel = 0);

    /**
     * @brief Turn switch OFF / Wyłącz przełącznik
     * @param ip Device IP address / Adres IP urządzenia
     * @param channel Switch channel (default 0) / Kanał przełącznika
     * @return true if successful / prawda jeśli sukces
     */
    static bool turnOff(const String& ip, int channel = 0);

    /**
     * @brief Toggle switch state / Przełącz stan przełącznika
     * @param ip Device IP address / Adres IP urządzenia
     * @param channel Switch channel (default 0) / Kanał przełącznika
     * @return true if successful / prawda jeśli sukces
     */
    static bool toggle(const String& ip, int channel = 0);

    /**
     * @brief Get switch state / Pobierz stan przełącznika
     * @param ip Device IP address / Adres IP urządzenia
     * @param channel Switch channel (default 0) / Kanał przełącznika
     * @param state Output state (true=ON, false=OFF) / Stan wyjściowy
     * @return true if successful / prawda jeśli sukces
     */
    static bool getState(const String& ip, int channel, bool& state);

    /**
     * @brief Check if device is reachable / Sprawdź czy urządzenie jest osiągalne
     * @param ip Device IP address / Adres IP urządzenia
     * @return true if reachable / prawda jeśli osiągalne
     */
    static bool ping(const String& ip);

    /**
     * @brief Get MAC address from device / Pobierz adres MAC z urządzenia
     * @param ip Device IP address / Adres IP urządzenia
     * @param mac Output MAC address / Adres MAC wyjściowy
     * @return true if successful / prawda jeśli sukces
     */
    static bool getMAC(const String& ip, String& mac);

private:
    /**
     * @brief Make RPC call to Shelly Gen4 / Wykonaj wywołanie RPC do Shelly Gen4
     * @param ip Device IP address / Adres IP urządzenia
     * @param method RPC method name / Nazwa metody RPC
     * @param params Parameters JSON / Parametry JSON
     * @param response Output response / Odpowiedź wyjściowa
     * @return true if successful / prawda jeśli sukces
     */
    static bool rpcCall(const String& ip, const String& method,
                       const JsonDocument& params, JsonDocument& response);

    /**
     * @brief Make HTTP GET request / Wykonaj żądanie HTTP GET
     * @param url Full URL / Pełny URL
     * @param response Output response / Odpowiedź wyjściowa
     * @return HTTP response code / Kod odpowiedzi HTTP
     */
    static int httpGet(const String& url, String& response);

    /**
     * @brief Make HTTP POST request / Wykonaj żądanie HTTP POST
     * @param url Full URL / Pełny URL
     * @param payload Request payload / Ładunek żądania
     * @param response Output response / Odpowiedź wyjściowa
     * @return HTTP response code / Kod odpowiedzi HTTP
     */
    static int httpPost(const String& url, const String& payload, String& response);
};

#endif // SHELLYCLIENT_H
