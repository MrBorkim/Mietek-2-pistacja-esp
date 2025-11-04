/**
 * @file LogicEngine.h
 * @brief Logic engine for rule-based device control with hysteresis
 *
 * Silnik logiki dla sterowania urządzeniami opartego na regułach z histerezą
 */

#ifndef LOGICENGINE_H
#define LOGICENGINE_H

#include <Arduino.h>
#include <vector>
#include "config.h"
#include "Storage.h"
#include "ShellyClient.h"

/**
 * @brief Rule execution state / Stan wykonania reguły
 */
struct RuleState {
    String ruleId;
    bool conditionMet;              // Is condition currently met / Czy warunek spełniony
    bool deviceState;               // Current device state / Aktualny stan urządzenia
    unsigned long conditionMetSince;// Time when condition first met / Czas pierwszego spełnienia
    unsigned long lastAction;       // Time of last action / Czas ostatniej akcji
    float lastValue;                // Last measured value / Ostatnia zmierzona wartość
    bool waitingForDelay;           // Waiting for delay / Oczekiwanie na opóźnienie
};

/**
 * @class LogicEngine
 * @brief Evaluates rules and controls devices based on conditions
 *
 * Ocenia reguły i steruje urządzeniami na podstawie warunków
 */
class LogicEngine {
public:
    /**
     * @brief Initialize logic engine / Inicjalizacja silnika logiki
     * @return true if successful / prawda jeśli sukces
     */
    bool begin();

    /**
     * @brief Main update loop / Główna pętla aktualizacji
     * Call regularly to evaluate rules / Wywołuj regularnie
     */
    void loop();

    /**
     * @brief Evaluate all rules / Oceń wszystkie reguły
     * @param exportPower Current export power (W) / Aktualna moc eksportu (W)
     */
    void evaluateRules(float exportPower);

    /**
     * @brief Evaluate single rule / Oceń pojedynczą regułę
     * @param rule Rule to evaluate / Reguła do oceny
     * @param currentValue Current measurement value / Aktualna wartość pomiaru
     * @return true if action was taken / prawda jeśli podjęto akcję
     */
    bool evaluateRule(const LogicRule& rule, float currentValue);

    /**
     * @brief Add or update rule / Dodaj lub zaktualizuj regułę
     * @param rule Rule to add/update / Reguła do dodania/aktualizacji
     */
    void updateRule(const LogicRule& rule);

    /**
     * @brief Remove rule / Usuń regułę
     * @param ruleId Rule ID / ID reguły
     */
    void removeRule(const String& ruleId);

    /**
     * @brief Reload rules from storage / Przeładuj reguły z pamięci
     */
    void reloadRules();

    /**
     * @brief Get rule state / Pobierz stan reguły
     * @param ruleId Rule ID / ID reguły
     * @param state Output state / Stan wyjściowy
     * @return true if rule found / prawda jeśli reguła znaleziona
     */
    bool getRuleState(const String& ruleId, RuleState& state);

    /**
     * @brief Get all rule states / Pobierz wszystkie stany reguł
     * @return Vector of rule states / Wektor stanów reguł
     */
    std::vector<RuleState> getAllRuleStates();

    /**
     * @brief Enable/disable rule / Włącz/wyłącz regułę
     * @param ruleId Rule ID / ID reguły
     * @param enabled Enable state / Stan włączenia
     */
    void setRuleEnabled(const String& ruleId, bool enabled);

    /**
     * @brief Manual device control (override rules) / Ręczne sterowanie (nadpisz reguły)
     * @param deviceId Device ID / ID urządzenia
     * @param state Desired state / Żądany stan
     * @return true if successful / prawda jeśli sukces
     */
    bool manualControl(const String& deviceId, bool state);

    /**
     * @brief Get statistics / Pobierz statystyki
     */
    void getStatistics(int& totalRules, int& activeRules, int& triggeredToday);

private:
    std::vector<LogicRule> rules;
    std::vector<RuleState> ruleStates;
    unsigned long lastEvaluation;
    int triggeredCount;

    /**
     * @brief Initialize rule state / Inicjalizuj stan reguły
     */
    void initializeRuleState(const LogicRule& rule);

    /**
     * @brief Find or create rule state / Znajdź lub utwórz stan reguły
     */
    RuleState& findOrCreateRuleState(const String& ruleId);

    /**
     * @brief Check if condition is met / Sprawdź czy warunek spełniony
     */
    bool checkCondition(const LogicRule& rule, float currentValue, const RuleState& state);

    /**
     * @brief Execute action (turn device on/off) / Wykonaj akcję
     */
    bool executeAction(const LogicRule& rule, bool turnOn, RuleState& state);

    /**
     * @brief Apply hysteresis to threshold / Zastosuj histerezę do progu
     */
    float applyHysteresis(float threshold, float hysteresis, bool currentState);

    /**
     * @brief Get device current state / Pobierz aktualny stan urządzenia
     */
    bool getDeviceState(const String& deviceId);

    /**
     * @brief Control device / Steruj urządzeniem
     */
    bool controlDevice(const String& deviceId, bool turnOn);

    /**
     * @brief Get device IP by ID / Pobierz IP urządzenia po ID
     */
    bool getDeviceIP(const String& deviceId, String& ip);
};

#endif // LOGICENGINE_H
