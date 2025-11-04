/**
 * @file ShellyClient.cpp
 * @brief Implementation of Shelly Gen4 device control
 *
 * Implementacja sterowania urządzeniami Shelly Gen4
 */

#include "ShellyClient.h"

bool ShellyClient::getDeviceInfo(const String& ip, JsonDocument& info) {
    JsonDocument params;
    return rpcCall(ip, "Shelly.GetDeviceInfo", params, info);
}

bool ShellyClient::getStatus(const String& ip, JsonDocument& status) {
    JsonDocument params;
    return rpcCall(ip, "Shelly.GetStatus", params, status);
}

bool ShellyClient::getMeasurement(const String& ip, ShellyMeasurement& measurement) {
    JsonDocument status;

    if (!getStatus(ip, status)) {
        measurement.valid = false;
        return false;
    }

    // Parse measurement data from status / Parsuj dane pomiarowe ze statusu
    // Shelly Gen4 structure: status.switch:0.apower, etc.
    if (status.containsKey("switch:0")) {
        JsonObject switchData = status["switch:0"];

        measurement.power_w = switchData["apower"] | 0.0f;
        measurement.voltage_v = switchData["voltage"] | 0.0f;
        measurement.current_a = switchData["current"] | 0.0f;
        measurement.energy_wh = switchData["aenergy"]["total"] | 0.0f;
        measurement.pf = switchData["pf"] | 0.0f;
        measurement.timestamp = millis();
        measurement.valid = true;

        return true;
    }

    measurement.valid = false;
    return false;
}

bool ShellyClient::turnOn(const String& ip, int channel) {
    JsonDocument params;
    params["id"] = channel;
    params["on"] = true;

    JsonDocument response;
    bool success = rpcCall(ip, "Switch.Set", params, response);

    if (success) {
        Serial.printf("[Shelly] Turned ON: %s (channel %d)\n", ip.c_str(), channel);
    }

    return success;
}

bool ShellyClient::turnOff(const String& ip, int channel) {
    JsonDocument params;
    params["id"] = channel;
    params["on"] = false;

    JsonDocument response;
    bool success = rpcCall(ip, "Switch.Set", params, response);

    if (success) {
        Serial.printf("[Shelly] Turned OFF: %s (channel %d)\n", ip.c_str(), channel);
    }

    return success;
}

bool ShellyClient::toggle(const String& ip, int channel) {
    JsonDocument params;
    params["id"] = channel;

    JsonDocument response;
    return rpcCall(ip, "Switch.Toggle", params, response);
}

bool ShellyClient::getState(const String& ip, int channel, bool& state) {
    JsonDocument status;

    if (!getStatus(ip, status)) {
        return false;
    }

    String switchKey = "switch:" + String(channel);
    if (status.containsKey(switchKey)) {
        state = status[switchKey]["output"] | false;
        return true;
    }

    return false;
}

bool ShellyClient::ping(const String& ip) {
    String url = "http://" + ip + "/rpc/Shelly.GetDeviceInfo";
    String response;

    int httpCode = httpGet(url, response);
    return (httpCode == 200);
}

bool ShellyClient::getMAC(const String& ip, String& mac) {
    JsonDocument info;

    if (!getDeviceInfo(ip, info)) {
        return false;
    }

    if (info.containsKey("mac")) {
        mac = info["mac"].as<String>();
        return true;
    }

    return false;
}

bool ShellyClient::rpcCall(const String& ip, const String& method,
                           const JsonDocument& params, JsonDocument& response) {
    String url = "http://" + ip + "/rpc/" + method;

    // Build JSON-RPC request / Zbuduj żądanie JSON-RPC
    JsonDocument request;
    request["jsonrpc"] = "2.0";
    request["id"] = 1;
    request["method"] = method;
    if (!params.isNull() && params.size() > 0) {
        request["params"] = params;
    }

    String payload;
    serializeJson(request, payload);

    String responseStr;
    int httpCode = httpPost(url, payload, responseStr);

    if (httpCode == 200) {
        DeserializationError error = deserializeJson(response, responseStr);
        if (error) {
            Serial.printf("[Shelly] JSON error: %s\n", error.c_str());
            return false;
        }

        // Check for RPC error / Sprawdź błąd RPC
        if (response.containsKey("error")) {
            Serial.printf("[Shelly] RPC error: %s\n",
                         response["error"]["message"].as<String>().c_str());
            return false;
        }

        // Extract result / Wyodrębnij wynik
        if (response.containsKey("result")) {
            response = response["result"];
        }

        return true;
    }

    Serial.printf("[Shelly] HTTP error %d for %s\n", httpCode, ip.c_str());
    return false;
}

int ShellyClient::httpGet(const String& url, String& response) {
    HTTPClient http;
    http.begin(url);
    http.setTimeout(SHELLY_HTTP_TIMEOUT);

    int httpCode = http.GET();
    if (httpCode > 0) {
        response = http.getString();
    }

    http.end();
    return httpCode;
}

int ShellyClient::httpPost(const String& url, const String& payload, String& response) {
    HTTPClient http;
    http.begin(url);
    http.setTimeout(SHELLY_HTTP_TIMEOUT);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(payload);
    if (httpCode > 0) {
        response = http.getString();
    }

    http.end();
    return httpCode;
}
