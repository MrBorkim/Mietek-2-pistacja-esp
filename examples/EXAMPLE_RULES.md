# Pistachio Smart Controller - Example Rules

This document contains practical automation rules for common solar + storage scenarios.

## 📋 Quick Start Examples

### Example 1: Basic Solar Surplus Utilization

**Goal**: Turn on water heater when exporting power to grid

**Rule Configuration**:
```json
{
  "name": "Water Heater - Solar Surplus",
  "enabled": true,
  "priority": 5,
  "conditions": [
    {
      "type": "grid_export",
      "operator": ">",
      "value": 1500,
      "duration": 60
    }
  ],
  "actions": [
    {
      "type": "device_control",
      "device": "water_heater",
      "state": true
    }
  ],
  "hysteresis": 300,
  "delayOn": 60,
  "delayOff": 300
}
```

**How it works**:
1. System monitors grid power
2. When exporting > 1500W for 60 seconds → turns ON water heater
3. Hysteresis 300W prevents oscillation
4. Turns OFF after 5 minutes if export drops below 1200W (1500 - 300)

---

### Example 2: Prevent Grid Import

**Goal**: Turn off non-essential loads when importing from grid

**Rule Configuration**:
```json
{
  "name": "Prevent Grid Import",
  "enabled": true,
  "priority": 10,
  "conditions": [
    {
      "type": "grid_import",
      "operator": ">",
      "value": 500,
      "duration": 30
    }
  ],
  "actions": [
    {
      "type": "device_control",
      "device": "water_heater",
      "state": false
    },
    {
      "type": "device_control",
      "device": "floor_heating",
      "state": false
    }
  ],
  "delayOff": 0
}
```

**How it works**:
- High priority (10) ensures it executes first
- Immediately turns OFF loads when importing > 500W for 30s
- No delay on turning OFF (safety feature)

---

### Example 3: Cascading Load Management

**Goal**: Activate devices in priority order based on available surplus

**Setup**: 3 rules with different priorities and thresholds

#### Priority 1: Water Heater (2kW)
```json
{
  "name": "Load Priority 1 - Water Heater",
  "priority": 1,
  "conditions": [
    {
      "type": "grid_export",
      "operator": ">",
      "value": 2000,
      "duration": 60
    }
  ],
  "actions": [
    {
      "type": "device_control",
      "device": "water_heater",
      "state": true
    }
  ],
  "hysteresis": 300,
  "delayOn": 60,
  "delayOff": 300
}
```

#### Priority 2: Floor Heating (3kW)
```json
{
  "name": "Load Priority 2 - Floor Heating",
  "priority": 2,
  "conditions": [
    {
      "type": "grid_export",
      "operator": ">",
      "value": 4000,
      "duration": 120
    },
    {
      "type": "device_state",
      "device": "water_heater",
      "state": true
    }
  ],
  "condition_logic": "AND",
  "actions": [
    {
      "type": "device_control",
      "device": "floor_heating",
      "state": true
    }
  ],
  "delayOn": 120,
  "delayOff": 600
}
```

#### Priority 3: Air Conditioning (2kW)
```json
{
  "name": "Load Priority 3 - AC",
  "priority": 3,
  "conditions": [
    {
      "type": "grid_export",
      "operator": ">",
      "value": 6000,
      "duration": 180
    },
    {
      "type": "time_range",
      "start": "10:00",
      "end": "18:00"
    }
  ],
  "condition_logic": "AND",
  "actions": [
    {
      "type": "device_control",
      "device": "air_conditioning",
      "state": true
    }
  ],
  "delayOn": 180,
  "delayOff": 300
}
```

**How it works**:
- Export > 2kW: Water heater turns ON
- Export > 4kW AND water heater ON: Floor heating turns ON
- Export > 6kW AND time 10:00-18:00: AC turns ON
- Loads turn OFF in reverse order as surplus decreases

---

### Example 4: Time Schedule

**Goal**: Turn off all devices at night

**Rule Configuration**:
```json
{
  "name": "Night Shutdown",
  "enabled": true,
  "priority": 1,
  "conditions": [
    {
      "type": "time_exact",
      "value": "22:00"
    }
  ],
  "actions": [
    {
      "type": "device_control",
      "device": "water_heater",
      "state": false
    },
    {
      "type": "device_control",
      "device": "floor_heating",
      "state": false
    },
    {
      "type": "device_control",
      "device": "air_conditioning",
      "state": false
    }
  ]
}
```

---

### Example 5: Smart Weekend Heating

**Goal**: Use solar surplus for heating on weekends

**Rule Configuration**:
```json
{
  "name": "Weekend Heating Boost",
  "enabled": true,
  "priority": 3,
  "conditions": [
    {
      "type": "grid_export",
      "operator": ">",
      "value": 2500,
      "duration": 90
    },
    {
      "type": "day_of_week",
      "value": 96
    },
    {
      "type": "time_range",
      "start": "08:00",
      "end": "20:00"
    }
  ],
  "condition_logic": "AND",
  "actions": [
    {
      "type": "device_control",
      "device": "floor_heating",
      "state": true
    }
  ],
  "delayOn": 90,
  "delayOff": 600
}
```

**Notes**:
- `day_of_week`: 96 = Saturday (64) + Sunday (32) bitmask
- Only active between 08:00-20:00
- Requires 2.5kW export for 90 seconds

---

## 🎯 Advanced Patterns

### Pattern 1: Intelligent Boiler Management

Multiple rules working together:

1. **Morning Priority**: Heat water 06:00-09:00 if any export
2. **Midday Boost**: Aggressive heating 10:00-16:00 with surplus
3. **Evening Conservation**: Only heat if large surplus after 17:00
4. **Safety Shutoff**: Never heat if importing > 500W

### Pattern 2: Three-Phase Load Balancing

For installations with Shelly 3EM:

1. Monitor per-phase power
2. Distribute loads across phases
3. Prevent phase overload
4. Optimize based on total export

---

## 📊 Real-World Scenario

### Installation Details
- **Solar**: 10kWp
- **Meter**: Shelly EM (grid connection)
- **Loads**:
  - Water Heater (2kW)
  - Floor Heating (3kW)
  - Pool Pump (1.5kW)
  - EV Charger (7kW - manual control)

### Rule Set

#### Rule 1: Water Heater (Always First)
- Priority: 1
- Condition: Export > 1500W for 60s
- Action: Turn ON water heater
- Hysteresis: 200W
- Result: Uses ~2kW of surplus

#### Rule 2: Floor Heating (If More Surplus)
- Priority: 2
- Condition: Export > 3500W for 120s AND water_heater = ON
- Action: Turn ON floor heating
- Result: Uses ~3kW additional surplus

#### Rule 3: Pool Pump (Daytime Only)
- Priority: 3
- Condition: Export > 5000W for 90s AND time 10:00-16:00
- Action: Turn ON pool pump
- Result: Uses ~1.5kW additional surplus

#### Rule 4: Emergency Shutoff
- Priority: 10 (highest)
- Condition: Import > 1000W for 20s
- Action: Turn OFF all managed devices
- Result: Prevents excessive grid import

### Expected Behavior

**Sunny Day (8kW production, 2kW base consumption)**:
- 10:00: 6kW surplus → Water heater ON
- 11:00: Surplus now 4kW → Floor heating ON
- 12:00: Surplus now 1kW (both running) → Pool pump waits
- 13:00: Production peak 10kW, surplus 5kW → Pool pump ON
- 15:00: Production drops to 6kW → Pool pump OFF (surplus < 5kW)
- 16:00: Production drops to 4kW → Floor heating OFF (surplus < 3.5kW)
- 18:00: Production 2kW → Water heater OFF (surplus < 1.3kW)

**Cloudy Day**:
- System remains in standby or only briefly activates water heater
- No unnecessary grid import

---

## 💡 Tips for Creating Rules

### Do's ✅
- Start with simple rules and test thoroughly
- Use adequate delays (60-180s) to prevent oscillation
- Set appropriate hysteresis (200-500W)
- Use priorities to control execution order
- Monitor for a few days before adding complex rules

### Don'ts ❌
- Don't set thresholds too low (< 500W unreliable)
- Don't use zero delays (causes oscillation)
- Don't create conflicting rules
- Don't forget to account for device startup current
- Don't activate all loads at once (stagger with priorities)

### Safety Considerations ⚠️
- Always include import prevention rule with high priority
- Set maximum number of simultaneous loads
- Use time ranges to prevent night activation
- Monitor voltage/frequency for anomalies
- Include manual override capability

---

## 🔧 Debugging Rules

### Check Rule Status
Access: `http://[ESP32_IP]/api/rules`

### Enable Debug Logging
In `config.h`:
```cpp
#define DEBUG_RULE_ENGINE true
#define DEBUG_VERBOSE true
```

### Test Rule Manually
Use curl to test API:
```bash
curl -X POST http://[ESP32_IP]/api/device/control \
  -H "Content-Type: application/json" \
  -d '{"deviceId":"water_heater","state":true,"manual":true}'
```

---

## 📚 Additional Resources

- **Web Interface**: Use the Rule Builder for visual rule creation
- **Templates**: Pre-made templates available in web UI
- **API Documentation**: See README.md for full API reference
- **Support**: GitHub Issues or Discord community

---

**Happy Automating! 🥜⚡**
