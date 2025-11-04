/**
 * @file RuleEngine.h
 * @brief Advanced rule engine for smart energy automation
 * @version 2.0.0
 * @author MrBorkim
 *
 * Features:
 * - Multi-condition rules with AND/OR/NOT logic
 * - Time-based automation
 * - Energy flow-based triggers (grid export/import)
 * - Priority-based execution
 * - Hysteresis and delays to prevent oscillation
 * - Device state management
 * - Rule scheduling and history
 *
 * Use Cases:
 * 1. Solar Surplus Utilization:
 *    IF grid_export > 1000W FOR 60s THEN turn_on(water_heater)
 *
 * 2. Grid Import Prevention:
 *    IF grid_import > 500W FOR 30s THEN turn_off(non_essential_loads)
 *
 * 3. Time + Energy Optimization:
 *    IF time BETWEEN 10:00-16:00 AND solar > 3kW THEN turn_on(heating)
 *
 * 4. Cascading Load Management:
 *    Priority 1: IF export > 2kW → water_heater
 *    Priority 2: IF export > 4kW → floor_heating
 *    Priority 3: IF export > 6kW → air_conditioning
 */

#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H

#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>
#include "config.h"
#include "EnergyMeterManager.h"

// ============================================================================
// Condition Types
// ============================================================================
enum class ConditionType {
    GRID_EXPORT,        // Grid export power > threshold
    GRID_IMPORT,        // Grid import power > threshold
    SOLAR_PRODUCTION,   // Solar production > threshold
    HOME_CONSUMPTION,   // Home consumption > threshold
    DEVICE_STATE,       // Device is ON/OFF
    TIME_RANGE,         // Time is within range
    TIME_EXACT,         // Exact time match
    DAY_OF_WEEK,        // Specific day(s) of week
    VOLTAGE,            // Voltage condition
    FREQUENCY,          // Frequency condition
    POWER_FACTOR,       // Power factor condition
    CUSTOM              // Custom expression
};

// ============================================================================
// Operators
// ============================================================================
enum class ComparisonOperator {
    GREATER_THAN,       // >
    LESS_THAN,          // <
    EQUAL,              // ==
    NOT_EQUAL,          // !=
    GREATER_OR_EQUAL,   // >=
    LESS_OR_EQUAL       // <=
};

// ============================================================================
// Logic Operators
// ============================================================================
enum class LogicOperator {
    AND,    // All conditions must be true
    OR,     // At least one condition must be true
    NOT     // Negate condition result
};

// ============================================================================
// Action Types
// ============================================================================
enum class ActionType {
    DEVICE_ON,          // Turn device ON
    DEVICE_OFF,         // Turn device OFF
    DEVICE_TOGGLE,      // Toggle device state
    SEND_NOTIFICATION,  // Send notification (future)
    LOG_EVENT,          // Log event
    CUSTOM              // Custom action
};

// ============================================================================
// Rule Condition Structure
// ============================================================================
struct RuleCondition {
    ConditionType type;
    ComparisonOperator op;

    // Numeric values
    float threshold;
    int duration;           // How long condition must be true (seconds)

    // Time-based
    String timeStart;       // HH:MM format
    String timeEnd;         // HH:MM format
    uint8_t dayOfWeek;      // Bitmask: Mon=1, Tue=2, ..., Sun=64

    // Device-based
    String deviceId;
    bool deviceState;

    // Custom
    String expression;

    // State tracking
    unsigned long trueStart;  // When condition became true
    bool currentlyTrue;

    RuleCondition() :
        type(ConditionType::GRID_EXPORT),
        op(ComparisonOperator::GREATER_THAN),
        threshold(0), duration(0),
        timeStart(""), timeEnd(""),
        dayOfWeek(0),
        deviceId(""), deviceState(false),
        expression(""),
        trueStart(0), currentlyTrue(false) {}
};

// ============================================================================
// Rule Action Structure
// ============================================================================
struct RuleAction {
    ActionType type;
    String deviceId;
    bool targetState;
    String message;
    String customData;

    RuleAction() :
        type(ActionType::DEVICE_ON),
        deviceId(""), targetState(true),
        message(""), customData("") {}
};

// ============================================================================
// Automation Rule Structure
// ============================================================================
struct AutomationRule {
    // Identification
    String id;
    String name;
    String description;

    // Configuration
    bool enabled;
    int priority;           // Lower number = higher priority (executed first)

    // Conditions
    std::vector<RuleCondition> conditions;
    LogicOperator conditionLogic;  // How to combine conditions

    // Actions
    std::vector<RuleAction> actions;

    // Advanced settings
    float hysteresis;       // Prevent oscillation (Watts)
    int delayOn;            // Delay before turning ON (seconds)
    int delayOff;           // Delay before turning OFF (seconds)

    // State tracking
    bool currentlyActive;   // Rule is currently triggering actions
    unsigned long activeSince;
    unsigned long lastTriggered;
    unsigned long triggerCount;

    // Timing for delays
    unsigned long delayOnStart;
    unsigned long delayOffStart;
    bool waitingForDelayOn;
    bool waitingForDelayOff;

    // Constructor
    AutomationRule() :
        id(""), name("Unnamed Rule"), description(""),
        enabled(true), priority(10),
        conditionLogic(LogicOperator::AND),
        hysteresis(DEFAULT_HYSTERESIS),
        delayOn(DEFAULT_DELAY_ON), delayOff(DEFAULT_DELAY_OFF),
        currentlyActive(false), activeSince(0),
        lastTriggered(0), triggerCount(0),
        delayOnStart(0), delayOffStart(0),
        waitingForDelayOn(false), waitingForDelayOff(false) {}
};

// ============================================================================
// Rule Engine Class
// ============================================================================
class RuleEngine {
public:
    RuleEngine();
    ~RuleEngine();

    // Initialization
    bool begin();
    void end();

    // Rule Management
    bool loadRules();
    bool saveRules();
    bool addRule(const AutomationRule& rule);
    bool updateRule(const String& ruleId, const AutomationRule& rule);
    bool deleteRule(const String& ruleId);
    bool enableRule(const String& ruleId, bool enabled);
    AutomationRule* getRule(const String& ruleId);
    std::vector<AutomationRule>& getRules() { return _rules; }
    int getRuleCount() const { return _rules.size(); }

    // Rule Evaluation
    void evaluateAllRules(const EnergyMeasurement& energy);
    bool evaluateRule(AutomationRule& rule, const EnergyMeasurement& energy);
    bool evaluateCondition(RuleCondition& condition, const EnergyMeasurement& energy);

    // Manual device control (overrides rules temporarily)
    bool manualControl(const String& deviceId, bool state);
    void clearManualOverrides();

    // Device state tracking
    bool getDeviceState(const String& deviceId);
    void setDeviceState(const String& deviceId, bool state);

    // Statistics
    int getActiveRuleCount() const;
    String getRuleStatus(const String& ruleId) const;
    void printRuleStatus() const;

    // Import/Export
    bool importRulesFromJson(const JsonDocument& doc);
    bool exportRulesToJson(JsonDocument& doc) const;

    // Helper: Create rule from template
    static AutomationRule createSurplusRule(const String& deviceId, float threshold, int delay);
    static AutomationRule createImportPreventRule(const String& deviceId, float threshold);
    static AutomationRule createTimeScheduleRule(const String& deviceId, const String& timeOn, const String& timeOff);
    static AutomationRule createPriorityLoadRule(const String& deviceId, float threshold, int priority);

private:
    // Rules storage
    std::vector<AutomationRule> _rules;

    // Device states
    std::map<String, bool> _deviceStates;
    std::map<String, bool> _manualOverrides;
    std::map<String, unsigned long> _lastActions;

    // Evaluation helpers
    bool checkConditionDuration(RuleCondition& condition);
    bool checkTimeRange(const String& start, const String& end);
    bool checkDayOfWeek(uint8_t mask);
    bool compareValues(float value, float threshold, ComparisonOperator op);

    // Action execution
    bool executeActions(const std::vector<RuleAction>& actions, const String& ruleId);
    bool executeAction(const RuleAction& action);

    // Hysteresis & Delays
    bool checkHysteresis(const AutomationRule& rule, float currentPower, float threshold);
    bool checkDelayOn(AutomationRule& rule);
    bool checkDelayOff(AutomationRule& rule);

    // Utility
    String generateRuleId();
    void sortRulesByPriority();
    void logRuleExecution(const String& ruleId, const String& action);
};

#endif // RULE_ENGINE_H
