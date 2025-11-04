/**
 * @file EnergyMeterManager.h
 * @brief Energy meter management for Shelly EM and PZEM-004T
 * @version 2.0.0
 * @author MrBorkim
 *
 * Supports:
 * - Shelly EM (single/dual channel) via HTTP/RPC
 * - Shelly 3EM (three-phase) via HTTP/RPC
 * - PZEM-004T-100A via Modbus RTU
 *
 * Features:
 * - Real-time power monitoring
 * - Grid export/import detection
 * - Energy calculation
 * - Historical data storage
 */

#ifndef ENERGY_METER_MANAGER_H
#define ENERGY_METER_MANAGER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

// ============================================================================
// Energy Meter Types
// ============================================================================
enum class MeterType {
    NONE,
    SHELLY_EM,      // Shelly EM (2 channels)
    SHELLY_3EM,     // Shelly 3EM (3 phases)
    PZEM_004T       // PZEM-004T-100A Modbus
};

// ============================================================================
// Measurement Data Structure
// ============================================================================
struct EnergyMeasurement {
    // Timestamp
    unsigned long timestamp;

    // Electrical parameters
    float voltage;          // Volts (V)
    float current;          // Amperes (A)
    float power;            // Watts (W) - positive = import, negative = export
    float apparentPower;    // Volt-Amperes (VA)
    float reactivePower;    // Volt-Amperes Reactive (VAR)
    float powerFactor;      // Power factor (0.0 - 1.0)
    float frequency;        // Hertz (Hz)

    // Energy counters
    float energyTotal;      // Total energy (kWh)
    float energyReturned;   // Returned energy (kWh) - for solar

    // Derived data
    GridFlowDirection flow; // STANDBY, EXPORT, IMPORT
    float gridExportPower;  // Watts being exported (positive)
    float gridImportPower;  // Watts being imported (positive)

    // Status
    bool valid;             // Measurement is valid
    String error;           // Error message if invalid

    // Constructor
    EnergyMeasurement() :
        timestamp(0), voltage(0), current(0), power(0),
        apparentPower(0), reactivePower(0), powerFactor(0), frequency(0),
        energyTotal(0), energyReturned(0),
        flow(GridFlowDirection::STANDBY),
        gridExportPower(0), gridImportPower(0),
        valid(false), error("") {}
};

// ============================================================================
// Meter Configuration
// ============================================================================
struct MeterConfig {
    MeterType type;

    // Network-based meters (Shelly EM/3EM)
    String mac;
    String ip;
    bool ct1_enabled;  // Channel 1 (Grid)
    bool ct2_enabled;  // Channel 2 (Solar)

    // Serial-based meters (PZEM)
    uint8_t modbusAddress;
    bool invertDirection;  // Invert power direction (for different wiring)

    // Calibration
    float voltageOffset;
    float currentOffset;
    float powerOffset;

    // Constructor
    MeterConfig() :
        type(MeterType::NONE),
        mac(""), ip(""),
        ct1_enabled(true), ct2_enabled(false),
        modbusAddress(PZEM_DEFAULT_ADDRESS), invertDirection(false),
        voltageOffset(0), currentOffset(0), powerOffset(0) {}
};

// ============================================================================
// Energy Meter Manager Class
// ============================================================================
class EnergyMeterManager {
public:
    EnergyMeterManager();
    ~EnergyMeterManager();

    // Initialization
    bool begin();
    void end();

    // Configuration
    bool loadConfig();
    bool saveConfig();
    bool setConfig(const MeterConfig& config);
    MeterConfig getConfig() const { return _config; }

    // Measurement
    bool readMeasurement();
    EnergyMeasurement getMeasurement() const { return _lastMeasurement; }
    bool isValid() const { return _lastMeasurement.valid; }

    // Grid flow detection
    GridFlowDirection getGridFlow() const { return _lastMeasurement.flow; }
    float getGridExportPower() const { return _lastMeasurement.gridExportPower; }
    float getGridImportPower() const { return _lastMeasurement.gridImportPower; }
    float getNetPower() const { return _lastMeasurement.power; }

    // Specific meter readings
    bool readShellyEM();
    bool readShelly3EM();
    bool readPZEM();

    // Utility
    String getMeterTypeString() const;
    void printMeasurement() const;
    unsigned long getLastReadTime() const { return _lastReadTime; }

private:
    // Configuration
    MeterConfig _config;

    // State
    EnergyMeasurement _lastMeasurement;
    unsigned long _lastReadTime;
    uint16_t _errorCount;

    // HTTP client for Shelly
    HTTPClient _http;

    // Helper methods
    void calculateGridFlow();
    void applyCalibration();
    bool validateMeasurement();

    // Shelly helpers
    bool fetchShellyRPC(const String& endpoint, JsonDocument& doc);
    bool parseShellyEMResponse(const JsonDocument& doc, int channel);
    bool parseShelly3EMResponse(const JsonDocument& doc);

    // PZEM helpers (if using PZEM library)
    bool initializePZEM();
    bool readPZEMData();

    // Error handling
    void setError(const String& error);
    void clearError();
};

#endif // ENERGY_METER_MANAGER_H
