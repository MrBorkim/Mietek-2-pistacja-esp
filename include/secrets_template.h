/**
 * @file secrets_template.h
 * @brief Template for WiFi credentials and sensitive configuration
 *
 * IMPORTANT:
 * 1. Copy this file to "secrets.h"
 * 2. Fill in your credentials
 * 3. DO NOT commit secrets.h to git (it's in .gitignore)
 *
 * Usage:
 *   cp include/secrets_template.h include/secrets.h
 *   nano include/secrets.h  # Edit with your credentials
 */

#ifndef SECRETS_H
#define SECRETS_H

// ============================================================================
// WiFi Credentials
// ============================================================================
// Default WiFi network to connect on boot
// Leave empty to start in AP mode for configuration
#define DEFAULT_WIFI_SSID "YourWiFiNetwork"
#define DEFAULT_WIFI_PASSWORD "YourWiFiPassword"

// ============================================================================
// Backend API Configuration (Optional)
// ============================================================================
// If you have a backend server for data aggregation
#define BACKEND_API_URL "http://192.168.1.100:5000/api"
#define BACKEND_API_KEY "your-secret-api-key"
#define BACKEND_USERNAME ""
#define BACKEND_PASSWORD ""

// ============================================================================
// OTA Update Password
// ============================================================================
// Password for Over-The-Air firmware updates
#define OTA_PASSWORD "pistachio-ota"

// ============================================================================
// Admin Password (Future Use)
// ============================================================================
// For web interface authentication (not implemented yet)
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "pistachio123"

#endif // SECRETS_H
