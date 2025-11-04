/**
 * @file Discovery.cpp
 * @brief Implementation of device discovery
 *
 * Implementacja odkrywania urządzeń
 */

#include "Discovery.h"

bool Discovery::begin() {
    Serial.println("[Discovery] Initializing...");

    lastDiscovery = 0;
    initialized = true;

    if (MDNS_ENABLE) {
        if (!MDNS.begin("pistachio")) {
            Serial.println("[Discovery] ERROR: mDNS initialization failed");
        } else {
            Serial.println("[Discovery] mDNS initialized");
        }
    }

    return true;
}

int Discovery::scanNetwork(std::vector<DiscoveredDevice>& devices) {
    Serial.println("[Discovery] Starting full network scan...");
    devices.clear();

    int count = 0;

    // Try mDNS first (fast) / Najpierw spróbuj mDNS (szybko)
    if (MDNS_ENABLE) {
        count += discoverMDNS(devices);
        Serial.printf("[Discovery] mDNS found %d devices\n", count);
    }

    // IP range scan (slower) / Skanowanie zakresu IP (wolniejsze)
    if (ARP_SCAN_ENABLE) {
        String networkPrefix = getNetworkPrefix();
        String startIP = networkPrefix + ".1";
        String endIP = networkPrefix + "." + String(NETWORK_SCAN_RANGE);

        int ipScanCount = scanIPRange(startIP, endIP, devices);
        Serial.printf("[Discovery] IP scan found %d new devices\n", ipScanCount);
        count += ipScanCount;
    }

    Serial.printf("[Discovery] Total devices discovered: %d\n", count);
    lastDiscovery = millis();

    return count;
}

int Discovery::discoverMDNS(std::vector<DiscoveredDevice>& devices) {
    Serial.println("[Discovery] Searching for Shelly devices via mDNS...");

    int count = 0;

    // Query for Shelly service / Zapytaj o usługę Shelly
    int n = MDNS.queryService("http", "tcp");

    for (int i = 0; i < n; i++) {
        String hostname = MDNS.hostname(i);

        // Check if it's a Shelly device / Sprawdź czy to Shelly
        if (hostname.indexOf("shelly") >= 0 || hostname.indexOf("Shelly") >= 0) {
            DiscoveredDevice device;
            device.ip = MDNS.IP(i).toString();
            device.hostname = hostname;
            device.type = "Shelly";
            device.isShelly = true;
            device.discoveredAt = millis();

            // Try to get MAC / Spróbuj pobrać MAC
            ShellyClient::getMAC(device.ip, device.mac);

            // Check if already in list / Sprawdź czy już na liście
            bool exists = false;
            for (const auto& existing : devices) {
                if (existing.ip == device.ip) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                devices.push_back(device);
                count++;
                Serial.printf("[Discovery] Found: %s (%s) - %s\n",
                             device.ip.c_str(), device.hostname.c_str(), device.mac.c_str());
            }
        }
    }

    return count;
}

int Discovery::scanIPRange(const String& startIP, const String& endIP,
                           std::vector<DiscoveredDevice>& devices) {
    Serial.printf("[Discovery] Scanning IP range: %s - %s\n", startIP.c_str(), endIP.c_str());

    uint8_t startOctets[4], endOctets[4];

    if (!parseIP(startIP, startOctets) || !parseIP(endIP, endOctets)) {
        Serial.println("[Discovery] ERROR: Invalid IP range");
        return 0;
    }

    int count = 0;

    // Scan range / Skanuj zakres
    for (int i = startOctets[3]; i <= endOctets[3]; i++) {
        String ip = String(startOctets[0]) + "." +
                   String(startOctets[1]) + "." +
                   String(startOctets[2]) + "." +
                   String(i);

        DiscoveredDevice device;
        if (probeIP(ip, device)) {
            // Check if already in list / Sprawdź czy już na liście
            bool exists = false;
            for (const auto& existing : devices) {
                if (existing.mac == device.mac && device.mac.length() > 0) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                devices.push_back(device);
                count++;
            }
        }

        // Yield to avoid watchdog / Oddaj sterowanie aby uniknąć watchdoga
        if (i % 10 == 0) {
            delay(10);
            yield();
        }
    }

    return count;
}

bool Discovery::probeIP(const String& ip, DiscoveredDevice& device) {
    String mac, type;

    if (isShellyDevice(ip, mac, type)) {
        device.ip = ip;
        device.mac = mac;
        device.type = type;
        device.isShelly = true;
        device.discoveredAt = millis();

        Serial.printf("[Discovery] Probed: %s - MAC: %s\n", ip.c_str(), mac.c_str());
        return true;
    }

    return false;
}

int Discovery::updateDeviceList(const std::vector<DiscoveredDevice>& discoveredDevices) {
    std::vector<ShellyDevice> storedDevices;
    Storage::loadDevices(storedDevices);

    int updatedCount = 0;

    for (const auto& discovered : discoveredDevices) {
        if (!discovered.isShelly || discovered.mac.length() == 0) {
            continue;
        }

        bool found = false;

        // Check if device exists by MAC / Sprawdź czy urządzenie istnieje po MAC
        for (auto& stored : storedDevices) {
            if (stored.mac.equalsIgnoreCase(discovered.mac)) {
                // Update IP if changed / Zaktualizuj IP jeśli zmienione
                if (stored.ip != discovered.ip) {
                    Serial.printf("[Discovery] IP changed for %s: %s -> %s\n",
                                 stored.name.c_str(), stored.ip.c_str(), discovered.ip.c_str());
                    stored.ip = discovered.ip;
                    updatedCount++;
                }
                stored.lastSeen = discovered.discoveredAt;
                found = true;
                break;
            }
        }

        // Add new device / Dodaj nowe urządzenie
        if (!found) {
            ShellyDevice newDevice;
            newDevice.id = "shelly_" + discovered.mac;
            newDevice.id.replace(":", "");
            newDevice.name = discovered.hostname.length() > 0 ?
                            discovered.hostname : discovered.type;
            newDevice.mac = discovered.mac;
            newDevice.ip = discovered.ip;
            newDevice.enabled = true;
            newDevice.lastSeen = discovered.discoveredAt;
            newDevice.lastPower = 0.0f;

            storedDevices.push_back(newDevice);
            updatedCount++;

            Serial.printf("[Discovery] New device added: %s (%s)\n",
                         newDevice.name.c_str(), newDevice.ip.c_str());
        }
    }

    if (updatedCount > 0) {
        Storage::saveDevices(storedDevices);
        Serial.printf("[Discovery] Updated %d device(s)\n", updatedCount);
    }

    return updatedCount;
}

bool Discovery::updateDeviceIP(const String& mac, const String& newIP) {
    ShellyDevice device;

    if (Storage::findDeviceByMAC(mac, device)) {
        if (device.ip != newIP) {
            Serial.printf("[Discovery] Updating IP for %s: %s -> %s\n",
                         device.name.c_str(), device.ip.c_str(), newIP.c_str());
            device.ip = newIP;
            device.lastSeen = millis();
            Storage::updateDevice(device);
            return true;
        }
    }

    return false;
}

void Discovery::periodicDiscovery() {
    if (millis() - lastDiscovery < DISCOVERY_INTERVAL) {
        return;
    }

    Serial.println("[Discovery] Running periodic discovery...");

    std::vector<DiscoveredDevice> devices;
    scanNetwork(devices);
    updateDeviceList(devices);
}

bool Discovery::parseIP(const String& ipStr, uint8_t octets[4]) {
    int parts[4];
    int count = sscanf(ipStr.c_str(), "%d.%d.%d.%d",
                      &parts[0], &parts[1], &parts[2], &parts[3]);

    if (count != 4) {
        return false;
    }

    for (int i = 0; i < 4; i++) {
        if (parts[i] < 0 || parts[i] > 255) {
            return false;
        }
        octets[i] = (uint8_t)parts[i];
    }

    return true;
}

bool Discovery::isShellyDevice(const String& ip, String& mac, String& type) {
    // Try to get device info / Spróbuj pobrać info o urządzeniu
    JsonDocument info;

    if (!ShellyClient::getDeviceInfo(ip, info)) {
        return false;
    }

    // Check if it's a Shelly device / Sprawdź czy to Shelly
    if (info.containsKey("app") && String(info["app"].as<String>()).indexOf("Shelly") >= 0) {
        mac = info["mac"].as<String>();
        type = info["model"].as<String>();
        return true;
    }

    return false;
}

String Discovery::getNetworkPrefix() {
    IPAddress localIP = WiFi.localIP();
    return String(localIP[0]) + "." + String(localIP[1]) + "." + String(localIP[2]);
}
