/**
 * @file Discovery.h
 * @brief Device discovery using mDNS, UPnP, and network scanning
 *
 * Odkrywanie urządzeń używając mDNS, UPnP i skanowania sieci
 */

#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <vector>
#include "config.h"
#include "Storage.h"
#include "ShellyClient.h"

/**
 * @brief Discovered device information / Informacje o odkrytym urządzeniu
 */
struct DiscoveredDevice {
    String ip;
    String mac;
    String hostname;
    String type;        // Device type / Typ urządzenia
    bool isShelly;
    unsigned long discoveredAt;
};

/**
 * @class Discovery
 * @brief Discovers Shelly devices on the network
 *
 * Odkrywa urządzenia Shelly w sieci
 */
class Discovery {
public:
    /**
     * @brief Initialize discovery service / Inicjalizacja usługi odkrywania
     * @return true if successful / prawda jeśli sukces
     */
    bool begin();

    /**
     * @brief Run full discovery scan / Uruchom pełne skanowanie odkrywania
     * @param devices Output vector of discovered devices / Wektor odkrytych urządzeń
     * @return Number of devices found / Liczba znalezionych urządzeń
     */
    int scanNetwork(std::vector<DiscoveredDevice>& devices);

    /**
     * @brief Discover devices using mDNS / Odkryj urządzenia używając mDNS
     * @param devices Output vector / Wektor wyjściowy
     * @return Number of devices found / Liczba znalezionych
     */
    int discoverMDNS(std::vector<DiscoveredDevice>& devices);

    /**
     * @brief Scan IP range for Shelly devices / Skanuj zakres IP dla urządzeń Shelly
     * @param startIP Starting IP (e.g., "192.168.1.1") / IP początkowe
     * @param endIP Ending IP (e.g., "192.168.1.254") / IP końcowe
     * @param devices Output vector / Wektor wyjściowy
     * @return Number of devices found / Liczba znalezionych
     */
    int scanIPRange(const String& startIP, const String& endIP,
                    std::vector<DiscoveredDevice>& devices);

    /**
     * @brief Check single IP for Shelly device / Sprawdź pojedynczy IP dla Shelly
     * @param ip IP address to check / Adres IP do sprawdzenia
     * @param device Output device info / Wyjściowe info urządzenia
     * @return true if Shelly device found / prawda jeśli znaleziono Shelly
     */
    bool probeIP(const String& ip, DiscoveredDevice& device);

    /**
     * @brief Update device list with discovered devices / Zaktualizuj listę urządzeń
     * @param discoveredDevices Discovered devices / Odkryte urządzenia
     * @return Number of devices updated / Liczba zaktualizowanych urządzeń
     */
    int updateDeviceList(const std::vector<DiscoveredDevice>& discoveredDevices);

    /**
     * @brief Find device by MAC and update IP / Znajdź urządzenie po MAC i zaktualizuj IP
     * @param mac MAC address / Adres MAC
     * @param newIP New IP address / Nowy adres IP
     * @return true if device found and updated / prawda jeśli znaleziono i zaktualizowano
     */
    bool updateDeviceIP(const String& mac, const String& newIP);

    /**
     * @brief Periodic discovery task / Okresowe zadanie odkrywania
     * Call this regularly to maintain device list / Wywołuj regularnie
     */
    void periodicDiscovery();

private:
    unsigned long lastDiscovery;
    bool initialized;

    /**
     * @brief Parse IP address to octets / Parsuj adres IP na oktety
     */
    bool parseIP(const String& ipStr, uint8_t octets[4]);

    /**
     * @brief Check if device is Shelly / Sprawdź czy urządzenie to Shelly
     */
    bool isShellyDevice(const String& ip, String& mac, String& type);

    /**
     * @brief Get local network prefix / Pobierz prefiks sieci lokalnej
     */
    String getNetworkPrefix();
};

#endif // DISCOVERY_H
