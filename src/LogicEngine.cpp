/**
 * @file LogicEngine.cpp
 * @brief Implementation of logic engine
 *
 * Implementacja silnika logiki
 */

#include "LogicEngine.h"

bool LogicEngine::begin() {
    Serial.println("[Logic] Initializing Logic Engine...");

    lastEvaluation = 0;
    triggeredCount = 0;

    // Load rules from storage / Wczytaj reguły z pamięci
    reloadRules();

    Serial.printf("[Logic] Loaded %d rule(s)\n", rules.size());
    return true;
}

void LogicEngine::loop() {
    if (millis() - lastEvaluation < LOGIC_UPDATE_INTERVAL) {
        return;
    }

    lastEvaluation = millis();

    // In real implementation, this would get actual export power from a meter
    // For now, this is just a placeholder
    // W prawdziwej implementacji to pobrałoby rzeczywistą moc eksportu z licznika
    // Na razie to tylko placeholder

    // evaluateRules() should be called with actual power reading
    // evaluateRules(actualExportPower);
}

void LogicEngine::evaluateRules(float exportPower) {
    for (auto& rule : rules) {
        if (!rule.enabled) {
            continue;
        }

        evaluateRule(rule, exportPower);
    }
}

bool LogicEngine::evaluateRule(const LogicRule& rule, float currentValue) {
    RuleState& state = findOrCreateRuleState(rule.id);
    state.lastValue = currentValue;

    // Get current device state / Pobierz aktualny stan urządzenia
    bool currentDeviceState = getDeviceState(rule.deviceId);
    state.deviceState = currentDeviceState;

    // Check condition with hysteresis / Sprawdź warunek z histerezą
    bool conditionMet = checkCondition(rule, currentValue, state);

    // Handle condition change / Obsłuż zmianę warunku
    if (conditionMet && !state.conditionMet) {
        // Condition just became true / Warunek właśnie stał się prawdziwy
        state.conditionMet = true;
        state.conditionMetSince = millis();
        state.waitingForDelay = true;

        Serial.printf("[Logic] Rule '%s': Condition MET (value=%.2f, threshold=%.2f)\n",
                     rule.id.c_str(), currentValue, rule.threshold);
    } else if (!conditionMet && state.conditionMet) {
        // Condition just became false / Warunek właśnie stał się fałszywy
        state.conditionMet = false;
        state.waitingForDelay = false;

        Serial.printf("[Logic] Rule '%s': Condition NOT MET (value=%.2f)\n",
                     rule.id.c_str(), currentValue);
    }

    // Execute action if condition met for long enough / Wykonaj akcję jeśli warunek spełniony wystarczająco długo
    if (state.conditionMet && state.waitingForDelay) {
        unsigned long timeInState = millis() - state.conditionMetSince;

        // Determine required delay based on desired action / Określ wymagane opóźnienie
        bool shouldTurnOn = (rule.condition == "export_above");
        unsigned long requiredDelay = shouldTurnOn ? rule.delayOn : rule.delayOff;

        if (timeInState >= requiredDelay) {
            // Execute action / Wykonaj akcję
            if (executeAction(rule, shouldTurnOn, state)) {
                state.waitingForDelay = false;
                state.lastAction = millis();
                triggeredCount++;
                return true;
            }
        } else {
            Serial.printf("[Logic] Rule '%s': Waiting for delay (%lu/%lu ms)\n",
                         rule.id.c_str(), timeInState, requiredDelay);
        }
    }

    // Handle turning off when condition no longer met / Obsłuż wyłączenie gdy warunek nie spełniony
    if (!state.conditionMet && currentDeviceState) {
        unsigned long timeSinceLastAction = millis() - state.lastAction;

        // Add delay before turning off / Dodaj opóźnienie przed wyłączeniem
        if (timeSinceLastAction >= rule.delayOff) {
            bool shouldTurnOff = (rule.condition == "export_above");
            if (shouldTurnOff) {
                executeAction(rule, false, state);
                state.lastAction = millis();
            }
        }
    }

    return false;
}

void LogicEngine::updateRule(const LogicRule& rule) {
    bool found = false;

    for (auto& r : rules) {
        if (r.id == rule.id) {
            r = rule;
            found = true;
            break;
        }
    }

    if (!found) {
        rules.push_back(rule);
        initializeRuleState(rule);
    }

    // Save to storage / Zapisz do pamięci
    Storage::saveRules(rules);

    Serial.printf("[Logic] Rule updated: %s\n", rule.id.c_str());
}

void LogicEngine::removeRule(const String& ruleId) {
    auto it = rules.begin();
    while (it != rules.end()) {
        if (it->id == ruleId) {
            it = rules.erase(it);
            Serial.printf("[Logic] Rule removed: %s\n", ruleId.c_str());
        } else {
            ++it;
        }
    }

    // Remove from states / Usuń ze stanów
    auto stateIt = ruleStates.begin();
    while (stateIt != ruleStates.end()) {
        if (stateIt->ruleId == ruleId) {
            stateIt = ruleStates.erase(stateIt);
        } else {
            ++stateIt;
        }
    }

    Storage::saveRules(rules);
}

void LogicEngine::reloadRules() {
    rules.clear();
    ruleStates.clear();

    Storage::loadRules(rules);

    // Initialize states for all rules / Inicjalizuj stany dla wszystkich reguł
    for (const auto& rule : rules) {
        initializeRuleState(rule);
    }
}

bool LogicEngine::getRuleState(const String& ruleId, RuleState& state) {
    for (const auto& s : ruleStates) {
        if (s.ruleId == ruleId) {
            state = s;
            return true;
        }
    }
    return false;
}

std::vector<RuleState> LogicEngine::getAllRuleStates() {
    return ruleStates;
}

void LogicEngine::setRuleEnabled(const String& ruleId, bool enabled) {
    for (auto& rule : rules) {
        if (rule.id == ruleId) {
            rule.enabled = enabled;
            Storage::saveRules(rules);
            Serial.printf("[Logic] Rule '%s' %s\n", ruleId.c_str(),
                         enabled ? "enabled" : "disabled");
            return;
        }
    }
}

bool LogicEngine::manualControl(const String& deviceId, bool state) {
    Serial.printf("[Logic] Manual control: %s -> %s\n",
                 deviceId.c_str(), state ? "ON" : "OFF");

    return controlDevice(deviceId, state);
}

void LogicEngine::getStatistics(int& totalRules, int& activeRules, int& triggeredToday) {
    totalRules = rules.size();
    activeRules = 0;
    triggeredToday = triggeredCount;

    for (const auto& rule : rules) {
        if (rule.enabled) {
            activeRules++;
        }
    }
}

void LogicEngine::initializeRuleState(const LogicRule& rule) {
    RuleState state;
    state.ruleId = rule.id;
    state.conditionMet = false;
    state.deviceState = false;
    state.conditionMetSince = 0;
    state.lastAction = 0;
    state.lastValue = 0.0f;
    state.waitingForDelay = false;

    ruleStates.push_back(state);
}

RuleState& LogicEngine::findOrCreateRuleState(const String& ruleId) {
    for (auto& state : ruleStates) {
        if (state.ruleId == ruleId) {
            return state;
        }
    }

    // Create new state / Utwórz nowy stan
    RuleState newState;
    newState.ruleId = ruleId;
    newState.conditionMet = false;
    newState.deviceState = false;
    newState.conditionMetSince = 0;
    newState.lastAction = 0;
    newState.lastValue = 0.0f;
    newState.waitingForDelay = false;

    ruleStates.push_back(newState);
    return ruleStates.back();
}

bool LogicEngine::checkCondition(const LogicRule& rule, float currentValue, const RuleState& state) {
    // Apply hysteresis / Zastosuj histerezę
    float effectiveThreshold = applyHysteresis(rule.threshold, rule.hysteresis, state.deviceState);

    if (rule.condition == "export_above") {
        return (currentValue >= effectiveThreshold);
    } else if (rule.condition == "export_below") {
        return (currentValue <= effectiveThreshold);
    }

    return false;
}

bool LogicEngine::executeAction(const LogicRule& rule, bool turnOn, RuleState& state) {
    Serial.printf("[Logic] Executing action for rule '%s': Turn %s device '%s'\n",
                 rule.id.c_str(), turnOn ? "ON" : "OFF", rule.deviceId.c_str());

    bool success = controlDevice(rule.deviceId, turnOn);

    if (success) {
        state.deviceState = turnOn;
        Serial.printf("[Logic] Action successful: %s is now %s\n",
                     rule.deviceId.c_str(), turnOn ? "ON" : "OFF");
    } else {
        Serial.printf("[Logic] Action failed for device: %s\n", rule.deviceId.c_str());
    }

    return success;
}

float LogicEngine::applyHysteresis(float threshold, float hysteresis, bool currentState) {
    float offset = threshold * hysteresis;

    if (currentState) {
        // Device is ON, apply negative hysteresis (lower threshold to turn off)
        // Urządzenie włączone, zastosuj ujemną histerezę (niższy próg do wyłączenia)
        return threshold - offset;
    } else {
        // Device is OFF, apply positive hysteresis (higher threshold to turn on)
        // Urządzenie wyłączone, zastosuj dodatnią histerezę (wyższy próg do włączenia)
        return threshold + offset;
    }
}

bool LogicEngine::getDeviceState(const String& deviceId) {
    String ip;
    if (!getDeviceIP(deviceId, ip)) {
        return false;
    }

    bool state = false;
    ShellyClient::getState(ip, 0, state);
    return state;
}

bool LogicEngine::controlDevice(const String& deviceId, bool turnOn) {
    String ip;
    if (!getDeviceIP(deviceId, ip)) {
        Serial.printf("[Logic] ERROR: Device not found: %s\n", deviceId.c_str());
        return false;
    }

    // Check current state first / Sprawdź najpierw aktualny stan
    bool currentState = false;
    if (ShellyClient::getState(ip, 0, currentState)) {
        Serial.printf("[Logic] Current state of %s: %s\n",
                     deviceId.c_str(), currentState ? "ON" : "OFF");

        // If already in desired state, skip control command / Jeśli już w żądanym stanie, pomiń komendę
        if (currentState == turnOn) {
            Serial.printf("[Logic] Device %s already %s - no action needed\n",
                         deviceId.c_str(), turnOn ? "ON" : "OFF");
            return true;
        }
    } else {
        Serial.printf("[Logic] Warning: Could not read current state of %s, proceeding anyway\n",
                     deviceId.c_str());
    }

    // Execute control command / Wykonaj komendę sterowania
    bool success = turnOn ? ShellyClient::turnOn(ip, 0) : ShellyClient::turnOff(ip, 0);

    if (success) {
        Serial.printf("[Logic] Device %s turned %s successfully\n",
                     deviceId.c_str(), turnOn ? "ON" : "OFF");
    } else {
        Serial.printf("[Logic] ERROR: Failed to turn %s device %s\n",
                     turnOn ? "ON" : "OFF", deviceId.c_str());
    }

    return success;
}

bool LogicEngine::getDeviceIP(const String& deviceId, String& ip) {
    std::vector<ShellyDevice> devices;
    Storage::loadDevices(devices);

    for (const auto& device : devices) {
        if (device.id == deviceId) {
            ip = device.ip;
            return (ip.length() > 0);
        }
    }

    return false;
}
