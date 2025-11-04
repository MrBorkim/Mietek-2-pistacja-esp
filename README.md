# 🥜 Pistachio Smart Controller - ESP32 Shelly Manager

![Version](https://img.shields.io/badge/version-2.0.0-brightgreen)
![Platform](https://img.shields.io/badge/platform-ESP32-blue)
![License](https://img.shields.io/badge/license-MIT-yellow)

**Ultra-nowoczesny, profesjonalny system zarządzania urządzeniami Shelly z zaawansowanym silnikiem reguł i integracją liczników energii.**

*Ultra-modern, professional Shelly device management system with advanced rule engine and energy meter integration.*

---

## 🌟 Nowe Funkcje v2.0 / New Features v2.0

### ✨ Ultra-Modern UI/UX
- 🎨 **Glassmorphism Design** - Nowoczesny, przejrzysty interfejs
- 🌓 **Dark/Light Mode** - Automatyczne przełączanie motywów
- 📊 **Real-time Charts** - Wykresy zużycia energii w czasie rzeczywistym
- 📱 **Fully Responsive** - Doskonała obsługa mobilna
- ⚡ **Smooth Animations** - Płynne animacje i przejścia
- 🎯 **Intuitive UX** - Zaprojektowane dla instalatorów

### 🧠 Advanced Rule Engine
- 📏 **Multi-condition Rules** - Złożone warunki logiczne (AND/OR/NOT)
- ⏱️ **Time-based Automation** - Harmonogramy czasowe
- 🔋 **Energy Flow Control** - Automatyka oparta na przepływie energii
- 🎚️ **Hysteresis Support** - Zapobiega oscylacjom
- ⏲️ **Delay Actions** - Opóźnienia włączania/wyłączania
- 🎯 **Priority System** - Priorytetyzacja reguł

### ⚡ Energy Meter Integration
- 📊 **Shelly EM Support** - Pełna integracja z licznikami Shelly EM
- 🔌 **PZEM-004T Support** - Wsparcie dla PZEM-004T-100A
- 📈 **Grid Export Detection** - Detekcja sprzedaży prądu do sieci
- 🔄 **Auto Relay Control** - Automatyczne sterowanie przekaźnikami
- 📉 **Power Threshold Management** - Zarządzanie progami mocy
- 💾 **Historical Data** - Archiwizacja danych energetycznych

### 🚀 Smart Automation
- 🌞 **Solar Optimization** - Optymalizacja wykorzystania energii słonecznej
- 💡 **Smart Load Management** - Inteligentne zarządzanie obciążeniem
- 🔄 **Auto-switching** - Automatyczne przełączanie źródeł energii
- 📊 **Energy Balance** - Bilansowanie produkcji i zużycia
- ⚠️ **Alert System** - System powiadomień i alertów
- 📲 **Remote Control** - Zdalne sterowanie przez API

---

## 📋 Spis Treści / Table of Contents

1. [Opis Projektu](#-opis-projektu--project-description)
2. [Wymagania](#-wymagania--requirements)
3. [Instalacja](#-instalacja--installation)
4. [Konfiguracja](#️-konfiguracja--configuration)
5. [Interfejs Użytkownika](#-interfejs-użytkownika--user-interface)
6. [System Zasad](#-system-zasad--rule-system)
7. [Integracja Liczników](#-integracja-liczników--energy-meter-integration)
8. [API Documentation](#-api-documentation)
9. [Przykłady Użycia](#-przykłady-użycia--usage-examples)
10. [Rozwój](#-rozwój--development)

---

## 📖 Opis Projektu / Project Description

**Polski:**

Pistachio Smart Controller to zaawansowany system zarządzania energią oparty na ESP32, specjalnie zaprojektowany dla instalacji fotowoltaicznych z urządzeniami Shelly. System umożliwia:

- **Inteligentne zarządzanie nadwyżkami energii** - Automatyczne włączanie odbiorników (bojler, ogrzewanie) gdy produkujesz więcej energii niż zużywasz
- **Zapobieganie sprzedaży energii** - System wykrywa kiedy zaczynasz sprzedawać prąd do sieci i włącza odbiorniki, aby wykorzystać energię lokalnie
- **Zaawansowane reguły automatyki** - Tworzenie złożonych zasad działania z wieloma warunkami
- **Monitoring w czasie rzeczywistym** - Śledzenie produkcji, zużycia i przepływu energii
- **Profesjonalny interfejs** - Nowoczesny, intuicyjny panel sterowania

**English:**

Pistachio Smart Controller is an advanced ESP32-based energy management system, specially designed for photovoltaic installations with Shelly devices. The system enables:

- **Smart surplus energy management** - Automatic activation of consumers (boiler, heating) when you produce more energy than you consume
- **Grid export prevention** - System detects when you start selling power to the grid and activates consumers to use energy locally
- **Advanced automation rules** - Create complex operating rules with multiple conditions
- **Real-time monitoring** - Track production, consumption, and energy flow
- **Professional interface** - Modern, intuitive control panel

---

## 🛠 Wymagania / Requirements

### Hardware / Sprzęt
- **ESP32** (min. 4MB Flash, zalecane 8MB)
- **Shelly Devices** (Plus 1PM, Plus 2PM, Pro 1PM, Pro 2PM)
- **Energy Meter** (jeden z poniższych):
  - Shelly EM (zalecane)
  - Shelly 3EM
  - PZEM-004T-100A-D-P(v1.0)
- **Stable WiFi** 2.4 GHz

### Software / Oprogramowanie
- PlatformIO Core 6.1+
- Python 3.8+
- Git

### Libraries / Biblioteki
```ini
ArduinoJson ^7.0.4
ESPAsyncWebServer
AsyncTCP
LittleFS (built-in)
ESPmDNS (built-in)
ModbusMaster (for PZEM)
```

---

## 📥 Instalacja / Installation

### Szybki Start / Quick Start

```bash
# 1. Klonuj repozytorium / Clone repository
git clone https://github.com/MrBorkim/Mietek-2-pistacja-esp.git
cd Mietek-2-pistacja-esp

# 2. Skopiuj szablon konfiguracji / Copy config template
cp include/secrets_template.h include/secrets.h

# 3. Edytuj secrets.h i wypełnij dane WiFi / Edit secrets.h with WiFi credentials
nano include/secrets.h

# 4. Zbuduj i wgraj / Build and upload
pio run --target upload

# 5. Wgraj system plików / Upload filesystem
pio run --target uploadfs

# 6. Monitoruj / Monitor
pio device monitor -b 115200
```

### Konfiguracja PlatformIO / PlatformIO Configuration

Plik `platformio.ini` jest już skonfigurowany. Możesz dostosować:

```ini
upload_port = /dev/cu.usbserial-110  # Zmień na swój port / Change to your port
monitor_speed = 115200
```

---

## ⚙️ Konfiguracja / Configuration

### 1. WiFi Setup

**Metoda A: Edycja secrets.h (przed wgraniem)**

```cpp
// include/secrets.h
#define DEFAULT_WIFI_SSID "TwojaSiec"
#define DEFAULT_WIFI_PASSWORD "TwojeHaslo"
```

**Metoda B: Tryb AP (po wgraniu)**

Jeśli WiFi nie jest skonfigurowane, ESP32 uruchomi Access Point:
- SSID: `Pistachio-Config-XXXXXX`
- Password: `pistachio123`
- IP: `192.168.4.1`

Otwórz `http://192.168.4.1` i skonfiguruj WiFi przez interfejs.

### 2. Energy Meter Configuration / Konfiguracja Licznika

#### Shelly EM / 3EM

1. Otwórz interfejs webowy ESP32
2. Przejdź do **Settings → Energy Meter**
3. Wybierz **Shelly EM**
4. Wprowadź MAC lub IP licznika
5. Kliknij **Add Meter**

Przykładowa konfiguracja JSON:
```json
{
  "type": "shelly_em",
  "mac": "AA:BB:CC:DD:EE:FF",
  "ip": "192.168.1.50",
  "ct1_enabled": true,
  "ct2_enabled": true
}
```

#### PZEM-004T-100A

1. Podłącz PZEM do ESP32:
   - PZEM TX → ESP32 GPIO 16 (RX2)
   - PZEM RX → ESP32 GPIO 17 (TX2)
   - PZEM GND → ESP32 GND
   - PZEM 5V → ESP32 5V

2. W interfejsie wybierz **PZEM-004T**
3. Ustaw adres Modbus (domyślnie 0x01)
4. Kliknij **Enable PZEM**

### 3. Adding Shelly Devices / Dodawanie Urządzeń Shelly

#### Auto-Discovery / Automatyczne Wykrywanie

1. **Devices → Discover Devices**
2. System automatycznie znajdzie urządzenia Shelly w sieci
3. Wybierz urządzenia do dodania
4. Nadaj im nazwy opisowe

#### Manual Add / Ręczne Dodawanie

1. **Devices → Add Manually**
2. Wprowadź:
   - **MAC Address**: `AA:BB:CC:DD:EE:FF`
   - **Device Name**: `Bojler Elektryczny`
   - **Device Type**: `Switch` / `Relay`
3. Kliknij **Add Device**

---

## 🎨 Interfejs Użytkownika / User Interface

### Dashboard

Główny panel pokazuje:
- 📊 **Energy Flow** - Produkcja, zużycie, eksport/import
- ⚡ **Current Power** - Aktualna moc w czasie rzeczywistym
- 🔄 **Active Rules** - Aktywne reguły automatyki
- 📈 **History Chart** - Wykres historyczny 24h
- ⚠️ **Alerts** - Powiadomienia i alerty

### Devices Panel

Zarządzanie urządzeniami:
- 📋 Lista wszystkich urządzeń Shelly
- 🔘 Przyciski ON/OFF z animacjami
- 📊 Aktualny pobór mocy każdego urządzenia
- ⏱️ Last seen / Online status
- ⚙️ Szybka konfiguracja

### Rules Builder

Zaawansowany kreator reguł:
- ➕ Dodawanie nowych reguł
- 📝 Edycja istniejących
- 🔀 Łączenie warunków (AND/OR/NOT)
- ⏰ Harmonogramy czasowe
- 🎯 Priorytety wykonania
- 🧪 Test mode - testowanie przed aktywacją

### Energy Meter Panel

Monitoring licznika energii:
- ⚡ Voltage, Current, Power, Energy
- 📈 Real-time graphs
- 📊 Daily/Weekly/Monthly statistics
- 💾 Export data to CSV
- 🔔 Alert configuration

### Settings

Zaawansowane ustawienia:
- 🌐 WiFi configuration
- 🔌 Energy meter setup
- 🔧 System configuration
- 📡 Backend API settings
- 🔄 OTA firmware update
- 📤 Export/Import configuration

---

## 🎯 System Zasad / Rule System

### Podstawowe Koncepty / Basic Concepts

Reguła (Rule) składa się z:
1. **Conditions** (Warunki) - Kiedy reguła ma się wykonać
2. **Actions** (Akcje) - Co ma się wydarzyć
3. **Priority** (Priorytet) - Kolejność wykonania
4. **Delays** (Opóźnienia) - Jak długo czekać przed akcją

### Typy Warunków / Condition Types

#### 1. Grid Export (Sprzedaż do sieci)
```
IF grid_export > 500W
THEN turn ON device "Bojler"
```

#### 2. Energy Production (Produkcja energii)
```
IF solar_production > 3000W
AND time BETWEEN 10:00-16:00
THEN turn ON device "Ogrzewanie"
```

#### 3. Power Threshold (Próg mocy)
```
IF grid_import > 2000W
THEN turn OFF device "Bojler"
```

#### 4. Time-based (Oparte na czasie)
```
IF time = 22:00
THEN turn OFF ALL devices
```

### Przykładowe Reguły / Example Rules

#### Reguła 1: Wykorzystaj Nadwyżki Energii
**Cel**: Włącz bojler gdy produkujesz nadmiar energii

```json
{
  "name": "Bojler - Nadwyżki PV",
  "enabled": true,
  "priority": 1,
  "conditions": [
    {
      "type": "grid_export",
      "operator": ">",
      "value": 1000,
      "duration": 60
    },
    {
      "type": "device_state",
      "device": "bojler",
      "state": false
    }
  ],
  "condition_logic": "AND",
  "actions": [
    {
      "type": "device_control",
      "device": "bojler",
      "state": true
    }
  ],
  "hysteresis": 200,
  "delay_on": 60,
  "delay_off": 300
}
```

**Wyjaśnienie**:
- Czeka 60 sekund z eksportem >1000W
- Włącza bojler
- Histereza 200W zapobiega oscylacjom
- Opóźnienie wyłączenia 5 minut

#### Reguła 2: Zapobiegaj Importowi
**Cel**: Wyłącz odbiorniki gdy importujesz z sieci

```json
{
  "name": "Wyłącz przy imporcie",
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
      "device": "bojler",
      "state": false
    },
    {
      "type": "device_control",
      "device": "ogrzewanie",
      "state": false
    }
  ],
  "delay_off": 0
}
```

#### Reguła 3: Inteligentne Ogrzewanie
**Cel**: Ogrzewanie tylko w słoneczne dni z nadwyżkami

```json
{
  "name": "Ogrzewanie - Smart",
  "enabled": true,
  "priority": 2,
  "conditions": [
    {
      "type": "grid_export",
      "operator": ">",
      "value": 2000,
      "duration": 120
    },
    {
      "type": "time_range",
      "start": "10:00",
      "end": "16:00"
    },
    {
      "type": "device_state",
      "device": "bojler",
      "state": true
    }
  ],
  "condition_logic": "AND",
  "actions": [
    {
      "type": "device_control",
      "device": "ogrzewanie",
      "state": true
    }
  ],
  "delay_on": 120,
  "delay_off": 600
}
```

---

## ⚡ Integracja Liczników / Energy Meter Integration

### Shelly EM / 3EM

#### Konfiguracja / Configuration

1. Zainstaluj licznik Shelly EM zgodnie z instrukcją producenta
2. Podłącz cęgi prądowe (CT):
   - **CT1**: Zasilanie z sieci (Grid)
   - **CT2**: Produkcja PV (Solar) - opcjonalnie

3. W interfejsie ESP32:
   - Dodaj licznik przez MAC lub IP
   - Skonfiguruj kierunki pomiaru
   - Ustaw kalibrację (jeśli potrzebna)

#### Obliczanie Eksportu / Export Calculation

```cpp
// Eksport = Produkcja - Zużycie
float grid_export = solar_production - total_consumption;

if (grid_export > threshold) {
    // Włącz odbiornik
    activateDevice("bojler");
}
```

#### API Shelly EM

System automatycznie odpytuje Shelly EM:
```
GET http://[shelly-em-ip]/rpc/EM.GetStatus?id=0
```

Odpowiedź:
```json
{
  "id": 0,
  "a_current": 2.45,
  "a_voltage": 230.1,
  "a_act_power": 563.5,
  "a_pf": 0.99,
  "a_freq": 50.0,
  "a_total_energy": 12345.6
}
```

### PZEM-004T-100A

#### Podłączenie / Wiring

```
PZEM-004T-100A → ESP32
--------------------------
TX (Yellow)    → GPIO 16 (RX2)
RX (Blue)      → GPIO 17 (TX2)
GND (Black)    → GND
5V (Red)       → 5V
```

#### Konfiguracja Modbus

```cpp
// include/config.h
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#define PZEM_SERIAL Serial2
#define PZEM_BAUD 9600
#define PZEM_ADDRESS 0x01
```

#### Odczyt Danych / Reading Data

System automatycznie odczytuje co 1 sekundę:
- Napięcie (V)
- Prąd (A)
- Moc czynna (W)
- Energia (kWh)
- Częstotliwość (Hz)
- Współczynnik mocy (PF)

#### Kalibracja

W interfejsie możesz skalibrować PZEM:
```
Settings → Energy Meter → PZEM Calibration
```

### Dual Meter Setup / Podwójny Licznik

Możesz użyć dwóch liczników jednocześnie:

**Setup 1: Shelly EM (Grid) + PZEM (Solar)**
```json
{
  "grid_meter": {
    "type": "shelly_em",
    "mac": "AA:BB:CC:DD:EE:FF",
    "channel": 1
  },
  "solar_meter": {
    "type": "pzem",
    "address": 0x01
  }
}
```

**Setup 2: Shelly 3EM (3-phase)**
```json
{
  "grid_meter": {
    "type": "shelly_3em",
    "mac": "AA:BB:CC:DD:EE:FF",
    "phases": [1, 2, 3]
  }
}
```

---

## 🌐 API Documentation

### REST API Endpoints

#### System Status

```http
GET /api/status
```

Response:
```json
{
  "version": "2.0.0",
  "uptime": 3600,
  "wifi": {
    "connected": true,
    "ssid": "MyWiFi",
    "ip": "192.168.1.100",
    "rssi": -45
  },
  "storage": {
    "total": 1048576,
    "used": 524288,
    "free": 524288
  }
}
```

#### Energy Data

```http
GET /api/energy/current
```

Response:
```json
{
  "timestamp": 1699999999,
  "grid": {
    "voltage": 230.5,
    "current": 2.45,
    "power": 564.7,
    "energy": 123.45,
    "flow": "export"
  },
  "solar": {
    "power": 3500.0,
    "energy_today": 28.5
  },
  "consumption": {
    "power": 2935.3,
    "energy_today": 42.3
  }
}
```

#### Device Control

```http
POST /api/device/control
Content-Type: application/json

{
  "deviceId": "shelly_aabbccddeeff",
  "state": true,
  "manual": true
}
```

#### Rule Management

```http
GET /api/rules
POST /api/rules
PUT /api/rules/{id}
DELETE /api/rules/{id}
```

#### Discovery

```http
POST /api/discovery/scan
```

Response:
```json
{
  "discovered": 5,
  "devices": [
    {
      "mac": "AA:BB:CC:DD:EE:FF",
      "ip": "192.168.1.50",
      "type": "shelly_plus_1pm",
      "name": "Shelly Plus 1PM"
    }
  ]
}
```

---

## 💡 Przykłady Użycia / Usage Examples

### Scenariusz 1: Dom z Fotowoltaiką + Bojler

**Setup:**
- Instalacja PV 5kWp
- Shelly EM na przyłączu
- Shelly Plus 1PM sterujący bojlerem 2kW
- ESP32 z Pistachio Controller

**Reguły:**
1. Jeśli eksport > 1500W przez 60s → włącz bojler
2. Jeśli import > 200W przez 30s → wyłącz bojler
3. Jeśli godzina > 22:00 → wyłącz bojler

**Rezultat:** Bojler grzeje wodę wyłącznie z nadwyżek PV, zerowy koszt energii.

### Scenariusz 2: Dom z PV + Ogrzewanie Elektryczne

**Setup:**
- Instalacja PV 10kWp
- Shelly 3EM (3-fazy)
- 3x Shelly Plus 2PM (ogrzewanie pomieszczeń)
- ESP32 z Pistachio Controller

**Reguły:**
1. **Priorytet 1**: Jeśli eksport > 3000W → włącz pokój 1
2. **Priorytet 2**: Jeśli eksport > 5000W → włącz pokój 2
3. **Priorytet 3**: Jeśli eksport > 7000W → włącz pokój 3
4. Jeśli import > 500W → wyłącz wszystko

**Rezultat:** Inteligentne ogrzewanie wykorzystujące nadwyżki PV z priorytetyzacją pomieszczeń.

### Scenariusz 3: Warształa z Maszynami

**Setup:**
- Instalacja PV 20kWp
- Shelly EM Pro
- Maszyny CNC, spawarka, kompresor (Shelly Pro 3EM)
- ESP32 z Pistachio Controller

**Reguły:**
1. Jeśli eksport > 10kW AND godzina 8-16 → zezwól na CNC
2. Jeśli eksport > 5kW → włącz kompresor (ładowanie zbiornika)
3. Jeśli import > 2kW → wyłącz kompresor

**Rezultat:** Automatyczne zarządzanie maszynami aby maksymalnie wykorzystać PV.

---

## 🔧 Rozwój / Development

### Kompilacja / Building

```bash
# Debug build
pio run -e esp32dev

# Release build
pio run -e esp32dev -t clean
pio run -e esp32dev

# Upload
pio run -t upload

# Upload filesystem
pio run -t uploadfs

# Monitor
pio device monitor
```

### Testowanie / Testing

```bash
# Unit tests
pio test

# Integration tests
pio test -e esp32dev

# Specific test
pio test -f test_rule_engine
```

### OTA Update

Po pierwszym wgraniu USB, możesz aktualizować przez WiFi:

```bash
# W platformio.ini:
upload_protocol = espota
upload_port = 192.168.1.100
upload_flags = --auth=pistachio-ota

# Upload:
pio run -t upload
```

### Debugowanie / Debugging

```cpp
// Enable debug logs
#define DEBUG_ENABLE true
#define DEBUG_VERBOSE true
#define DEBUG_RULE_ENGINE true
#define DEBUG_ENERGY_METER true
```

Logi na Serial:
```
[EnergyMeter] Grid: 564.7W (export)
[RuleEngine] Rule "Bojler - Nadwyżki PV" triggered
[RuleEngine] Activating device: bojler
[ShellyClient] POST http://192.168.1.50/rpc/Switch.Set
[LogicEngine] Device bojler state changed: OFF → ON
```

---

## 📚 Dokumentacja Techniczna / Technical Documentation

### Struktura Projektu / Project Structure

```
Mietek-2-pistacja-esp/
├── include/                    # Header files
│   ├── config.h               # Main configuration
│   ├── secrets.h              # WiFi credentials
│   └── ...
├── src/                       # Source code
│   ├── main.cpp              # Main application
│   ├── EnergyMeter.cpp       # Energy meter manager
│   ├── RuleEngine.cpp        # Advanced rule engine
│   ├── ShellyClient.cpp      # Shelly communication
│   └── ...
├── lib/                       # External libraries
├── data/                      # Web interface files
│   ├── index.html            # Main UI
│   ├── style.css             # Stylesheet
│   ├── script.js             # JavaScript
│   └── ...
├── docs/                      # Documentation
├── examples/                  # Example configurations
├── test/                      # Unit tests
├── platformio.ini            # PlatformIO config
└── README.md                 # This file
```

### Architecture / Architektura

```
┌─────────────────────────────────────────┐
│         Web Interface (HTML/JS)         │
│   (Dashboard, Rules Builder, Settings)  │
└────────────────┬────────────────────────┘
                 │ HTTP/WebSocket
┌────────────────▼────────────────────────┐
│       AsyncWebServer (ESP32)            │
│        REST API Endpoints               │
└────────────────┬────────────────────────┘
                 │
┌────────────────▼────────────────────────┐
│         Main Application                │
│  ┌──────────────────────────────────┐   │
│  │      Rule Engine                 │   │
│  │  (Evaluate & Execute Rules)      │   │
│  └──────────────────────────────────┘   │
│  ┌──────────────────────────────────┐   │
│  │    Energy Meter Manager          │   │
│  │  (Shelly EM / PZEM Reader)       │   │
│  └──────────────────────────────────┘   │
│  ┌──────────────────────────────────┐   │
│  │     Shelly Client                │   │
│  │  (Device Control & Status)       │   │
│  └──────────────────────────────────┘   │
│  ┌──────────────────────────────────┐   │
│  │     Discovery Service            │   │
│  │  (mDNS + Network Scan)           │   │
│  └──────────────────────────────────┘   │
│  ┌──────────────────────────────────┐   │
│  │     Storage Manager              │   │
│  │  (LittleFS Config & Data)        │   │
│  └──────────────────────────────────┘   │
└─────────────────────────────────────────┘
                 │
┌────────────────▼────────────────────────┐
│      Hardware Layer                     │
│  ┌──────────┐ ┌──────────┐ ┌─────────┐ │
│  │ Shelly   │ │Shelly EM │ │ PZEM    │ │
│  │ Devices  │ │ (WiFi)   │ │(Serial) │ │
│  └──────────┘ └──────────┘ └─────────┘ │
└─────────────────────────────────────────┘
```

### Rule Engine Flow / Przepływ Silnika Reguł

```
1. Energy Meter Update (1s interval)
   ↓
2. Read Current Grid Power
   ↓
3. Rule Engine Evaluation
   ├─ Check all enabled rules
   ├─ Evaluate conditions (AND/OR/NOT)
   ├─ Check delays & hysteresis
   └─ Execute actions by priority
   ↓
4. Device State Update
   ├─ Send commands to Shelly
   ├─ Update device status
   └─ Log state changes
   ↓
5. Store Metrics
   └─ Save to LittleFS
```

---

## ❓ FAQ / Często Zadawane Pytania

### Q1: Jaki licznik energii wybrać?

**A:** Zalecamy **Shelly EM** dla łatwości instalacji i integracji. PZEM-004T to tańsza alternatywa, ale wymaga połączenia szeregowego.

### Q2: Czy mogę używać kilku ESP32?

**A:** Tak, możesz mieć wiele kontrolerów ESP32 w różnych lokalizacjach, każdy zarządzający swoją grupą urządzeń.

### Q3: Jak działa detekcja eksportu energii?

**A:** System odczytuje moc z licznika. Jeśli wartość jest ujemna (lub dodatnia w zależności od konfiguracji), oznacza to eksport do sieci.

### Q4: Co to jest histereza?

**A:** Histereza zapobiega oscylacjom. Np. jeśli próg włączenia to 1000W, a histereza 200W, urządzenie wyłączy się dopiero przy 800W, nie zaraz po spadku poniżej 1000W.

### Q5: Czy system działa offline?

**A:** Tak, wszystkie reguły działają lokalnie na ESP32. Połączenie z internetem potrzebne jest tylko do zdalnego dostępu i backendu (opcjonalnie).

### Q6: Jak zaktualizować firmware?

**A:** Przez OTA w interfejsie webowym (Settings → Firmware Update) lub przez USB z PlatformIO.

### Q7: Jakie są limity systemu?

**A:**
- Maks. 32 urządzenia Shelly
- Maks. 50 reguł automatyki
- Polling interval: 1-60s (konfigurowalny)
- Historia: 7 dni (konfigurowalne)

### Q8: Czy współpracuje z Home Assistant?

**A:** Tak, możesz integrować przez REST API lub MQTT (w przygotowaniu).

---

## 🤝 Contributing / Współpraca

Zapraszamy do współpracy!

1. Fork projektu
2. Stwórz branch (`git checkout -b feature/AmazingFeature`)
3. Commit zmian (`git commit -m 'Add AmazingFeature'`)
4. Push do brancha (`git push origin feature/AmazingFeature`)
5. Otwórz Pull Request

---

## 📄 Licencja / License

Ten projekt jest dostępny na licencji MIT. Zobacz plik `LICENSE`.

---

## 👤 Autor / Author

**MrBorkim**
- GitHub: [@MrBorkim](https://github.com/MrBorkim)

---

## 🙏 Podziękowania / Acknowledgments

- Społeczność ESP32
- Twórcy ArduinoJson
- Zespół Shelly za doskonałe urządzenia i API
- Wszyscy testerzy i kontrybutorzy

---

## 📞 Wsparcie / Support

- 📧 Email: support@pistachio-controller.local
- 💬 Discord: [Join our server](#)
- 🐛 Issues: [GitHub Issues](https://github.com/MrBorkim/Mietek-2-pistacja-esp/issues)
- 📚 Wiki: [Project Wiki](https://github.com/MrBorkim/Mietek-2-pistacja-esp/wiki)

---

## 🗺️ Roadmap

### v2.1 (Q1 2025)
- [ ] MQTT support
- [ ] Home Assistant integration
- [ ] Mobile app (iOS/Android)
- [ ] Cloud sync (optional)
- [ ] Advanced analytics

### v2.2 (Q2 2025)
- [ ] Machine learning power prediction
- [ ] Weather forecast integration
- [ ] Multi-language support
- [ ] Voice control (Alexa/Google)

### v3.0 (Q3 2025)
- [ ] Support for other devices (Tasmota, ESPHome)
- [ ] Distributed system (multiple ESP32)
- [ ] Professional installer portal
- [ ] Billing and reports

---

## 📊 Statystyki / Stats

![GitHub stars](https://img.shields.io/github/stars/MrBorkim/Mietek-2-pistacja-esp)
![GitHub forks](https://img.shields.io/github/forks/MrBorkim/Mietek-2-pistacja-esp)
![GitHub issues](https://img.shields.io/github/issues/MrBorkim/Mietek-2-pistacja-esp)
![GitHub license](https://img.shields.io/github/license/MrBorkim/Mietek-2-pistacja-esp)

---

**Zbudujmy razem lepszą przyszłość energetyczną! 🌞⚡🥜**

**Let's build a better energy future together! 🌞⚡🥜**
