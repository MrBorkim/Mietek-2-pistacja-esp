/**
 * @file RESTClient.cpp
 * @brief Implementation of REST API client
 *
 * Implementacja klienta REST API
 */

#include "RESTClient.h"

// Base64 encoding helper for Basic Auth
String base64Encode(const String& input) {
    const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    String output;
    int val = 0;
    int valb = -6;

    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            output += base64_chars[(val >> valb) & 0x3F];
            valb -= 6;
        }
    }

    if (valb > -6) {
        output += base64_chars[((val << 8) >> (valb + 8)) & 0x3F];
    }

    while (output.length() % 4) {
        output += '=';
    }

    return output;
}

bool RESTClient::begin(const BackendConfig& cfg) {
    config = cfg;
    Serial.printf("[REST] Initialized with base URL: %s\n", config.baseUrl.c_str());
    return true;
}

bool RESTClient::getDeviceConfig(const String& deviceId, JsonDocument& response) {
    String endpoint = "/devices/" + deviceId;
    String responseStr;

    int httpCode = httpGet(endpoint, responseStr);

    if (httpCode == 200) {
        DeserializationError error = deserializeJson(response, responseStr);
        if (error) {
            Serial.printf("[REST] JSON deserialization error: %s\n", error.c_str());
            return false;
        }
        Serial.printf("[REST] Device config fetched for: %s\n", deviceId.c_str());
        return true;
    }

    Serial.printf("[REST] Failed to get device config, HTTP code: %d\n", httpCode);
    return false;
}

bool RESTClient::postDeviceState(const String& deviceId, const JsonDocument& state) {
    String endpoint = "/devices/" + deviceId + "/state";
    String payload;
    serializeJson(state, payload);

    String response;
    int httpCode = httpPost(endpoint, payload, response);

    if (httpCode >= 200 && httpCode < 300) {
        Serial.printf("[REST] Device state posted for: %s\n", deviceId.c_str());
        return true;
    }

    Serial.printf("[REST] Failed to post device state, HTTP code: %d\n", httpCode);
    return false;
}

bool RESTClient::getSettings(JsonDocument& settings) {
    String endpoint = "/settings";
    String responseStr;

    int httpCode = httpGet(endpoint, responseStr);

    if (httpCode == 200) {
        DeserializationError error = deserializeJson(settings, responseStr);
        if (error) {
            Serial.printf("[REST] JSON deserialization error: %s\n", error.c_str());
            return false;
        }
        Serial.println("[REST] Settings fetched successfully");
        return true;
    }

    Serial.printf("[REST] Failed to get settings, HTTP code: %d\n", httpCode);
    return false;
}

bool RESTClient::postSettings(const JsonDocument& settings) {
    String endpoint = "/settings";
    String payload;
    serializeJson(settings, payload);

    String response;
    int httpCode = httpPost(endpoint, payload, response);

    if (httpCode >= 200 && httpCode < 300) {
        Serial.println("[REST] Settings posted successfully");
        return true;
    }

    Serial.printf("[REST] Failed to post settings, HTTP code: %d\n", httpCode);
    return false;
}

bool RESTClient::ping() {
    String endpoint = "/ping";
    String response;

    int httpCode = httpGet(endpoint, response);
    return (httpCode == 200);
}

void RESTClient::setConfig(const BackendConfig& cfg) {
    config = cfg;
}

int RESTClient::httpGet(const String& endpoint, String& response) {
    String url = buildURL(endpoint);

    Serial.printf("[REST] GET %s\n", url.c_str());

    http.begin(url);
    http.setTimeout(REST_TIMEOUT);
    addAuthHeaders();

    int httpCode = http.GET();
    if (httpCode > 0) {
        response = http.getString();
    }

    http.end();
    return httpCode;
}

int RESTClient::httpPost(const String& endpoint, const String& payload, String& response) {
    String url = buildURL(endpoint);

    Serial.printf("[REST] POST %s\n", url.c_str());

    http.begin(url);
    http.setTimeout(REST_TIMEOUT);
    http.addHeader("Content-Type", "application/json");
    addAuthHeaders();

    int httpCode = http.POST(payload);
    if (httpCode > 0) {
        response = http.getString();
    }

    http.end();
    return httpCode;
}

void RESTClient::addAuthHeaders() {
    if (config.token.length() > 0) {
        // Token-based authentication / Autoryzacja oparta na tokenie
        http.addHeader("Authorization", "Bearer " + config.token);
    } else if (config.username.length() > 0 && config.password.length() > 0) {
        // Basic authentication / Autoryzacja podstawowa
        String auth = config.username + ":" + config.password;
        String authEncoded = base64Encode(auth);
        http.addHeader("Authorization", "Basic " + authEncoded);
    }
}

String RESTClient::buildURL(const String& endpoint) {
    String url = config.baseUrl;

    // Ensure no double slashes / Upewnij się, że nie ma podwójnych slashy
    if (url.endsWith("/") && endpoint.startsWith("/")) {
        url = url.substring(0, url.length() - 1);
    } else if (!url.endsWith("/") && !endpoint.startsWith("/")) {
        url += "/";
    }

    url += endpoint;
    return url;
}
