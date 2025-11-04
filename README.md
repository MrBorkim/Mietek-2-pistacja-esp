# 🥜 Pistachio Smart Controller - ESP32 Shelly Manager

Inteligentny kontroler ESP32 do zarządzania urządzeniami Shelly z automatycznym wykrywaniem i zarządzaniem mocą na podstawie reguł logicznych.

ESP32-based smart controller for managing Shelly devices with auto-discovery and rule-based power management.

---

## 📋 Spis Treści / Table of Contents

- [Opis / Description](#-opis--description)
- [Funkcje / Features](#-funkcje--features)
- [Wymagania / Requirements](#-wymagania--requirements)
- [Instalacja / Installation](#-instalacja--installation)
- [Konfiguracja / Configuration](#-konfiguracja--configuration)
- [Użycie / Usage](#-użycie--usage)
- [Struktura Projektu / Project Structure](#-struktura-projektu--project-structure)
- [API Endpoints](#-api-endpoints)
- [Rozwój / Development](#-rozwój--development)

---

## 📖 Opis / Description

**Polski:**
Pistachio Smart Controller to zaawansowany system zarządzania urządzeniami Shelly oparty na ESP32. Projekt umożliwia:
- Automatyczne wykrywanie urządzeń Shelly w sieci lokalnej (mDNS i skanowanie)
- Zdalne sterowanie urządzeniami przez interfejs webowy
- Zarządzanie mocą na podstawie definiowanych reguł logicznych
- Monitorowanie zużycia energii
- Konfigurację WiFi przez Access Point
- Aktualizacje OTA (Over-The-Air)

**English:**
Pistachio Smart Controller is an advanced ESP32-based management system for Shelly devices. The project enables:
- Automatic discovery of Shelly devices on local network (mDNS and scanning)
- Remote device control via web interface
- Rule-based power management
- Energy consumption monitoring
- WiFi configuration via Access Point
- OTA (Over-The-Air) updates

---

## ✨ Funkcje / Features

### Podstawowe / Core Features
- ✅ **Auto-Discovery**: Automatyczne wykrywanie urządzeń Shelly przez mDNS i skanowanie sieci
- ✅ **Interfejs WWW**: Nowoczesny, responsywny interfejs zarządzania
- ✅ **Sterowanie Urządzeniami**: Włączanie/wyłączanie urządzeń Shelly
- ✅ **Monitoring Mocy**: Śledzenie zużycia energii w czasie rzeczywistym
- ✅ **Konfiguracja WiFi**: Łatwa konfiguracja przez tryb Access Point
- ✅ **Przechowywanie Konfiguracji**: Zapisywanie ustawień w LittleFS
- ✅ **REST API**: Pełne API do integracji z innymi systemami

### Zaawansowane / Advanced Features
- 🔧 **Logic Engine**: Silnik reguł do automatycznego zarządzania mocą
- 🔧 **Backend Sync**: Synchronizacja z zewnętrznym backendem REST
- 🔧 **OTA Updates**: Bezprzewodowe aktualizacje firmware
- 🔧 **Telemetria**: Wysyłanie danych telemetrycznych
- 🔧 **Multi-Device**: Obsługa wielu urządzeń Shelly jednocześnie

---

## 🛠 Wymagania / Requirements

### Sprzęt / Hardware
- **ESP32** (dowolny model z co najmniej 4MB Flash)
- Połączenie WiFi
- Urządzenia **Shelly Gen4** w tej samej sieci lokalnej

### Oprogramowanie / Software
- [PlatformIO](https://platformio.org/) (zalecane / recommended)
- Arduino IDE (alternatywnie / alternatively)
- Python 3.x (dla PlatformIO)

### Biblioteki / Libraries
- `ArduinoJson` (^7.0.4)
- `ESPAsyncWebServer`
- `AsyncTCP`
- `LittleFS` (wbudowane / built-in)
- `ESPmDNS` (wbudowane / built-in)

---

## 📥 Instalacja / Installation

### Metoda 1: PlatformIO (Zalecana / Recommended)

```bash
# 1. Sklonuj repozytorium / Clone repository
git clone https://github.com/MrBorkim/Mietek-2-pistacja-esp.git
cd Mietek-2-pistacja-esp

# 2. Zainstaluj PlatformIO jeśli nie masz / Install PlatformIO if needed
pip install platformio

# 3. Skonfiguruj secrets.h / Configure secrets.h
# Edytuj src/secrets.h i wypełnij dane WiFi / Edit src/secrets.h and fill WiFi credentials

# 4. Zbuduj i wgraj / Build and upload
pio run --target upload

# 5. Monitoruj port szeregowy / Monitor serial port
pio device monitor
```

### Metoda 2: Arduino IDE

```bash
# 1. Pobierz projekt / Download project
# Pobierz jako ZIP lub sklonuj / Download as ZIP or clone

# 2. Otwórz src/main.cpp w Arduino IDE

# 3. Zainstaluj wymagane biblioteki przez Library Manager:
#    - ArduinoJson (wersja 7.x)
#    - ESPAsyncWebServer
#    - AsyncTCP

# 4. Wybierz płytkę ESP32 Dev Module

# 5. Skonfiguruj src/secrets.h

# 6. Kompiluj i wgraj
```

---

## ⚙️ Konfiguracja / Configuration

### 1. Konfiguracja WiFi / WiFi Configuration

Edytuj `src/secrets.h`:

```cpp
#define DEFAULT_WIFI_SSID "Twoja_Siec_WiFi"
#define DEFAULT_WIFI_PASSWORD "TwojeHaslo"
```

**Tryb AP / AP Mode:**
Jeśli pozostawisz puste pola WiFi, ESP32 uruchomi się w trybie Access Point:
- **SSID**: `Pistachio-Config-XXXXXX` (XXXXXX = część MAC)
- **Hasło**: `pistachio123`
- **IP**: `192.168.4.1`

### 2. Konfiguracja Urządzeń / Device Configuration

#### Automatyczne Wykrywanie / Auto-Discovery
1. Otwórz interfejs webowy
2. Przejdź do zakładki **Devices**
3. Kliknij **"Discover Devices"**
4. System automatycznie znajdzie urządzenia Shelly w sieci

#### Ręczne Dodawanie / Manual Adding
1. W interfejsie wybierz **"Add Device Manually"**
2. Wprowadź:
   - **MAC Address**: Adres MAC urządzenia Shelly (format: `AA:BB:CC:DD:EE:FF`)
   - **Device Name**: Opis urządzenia (np. "Bojler", "Ogrzewanie")
3. Kliknij **"Add Device"**
4. IP urządzenia zostanie automatycznie wykryte

### 3. Przykładowe Pliki Konfiguracyjne / Example Config Files

W katalogu `data/` znajdziesz przykłady:
- `example-config.json` - przykładowa konfiguracja WiFi
- `example-devices.json` - przykładowa lista urządzeń

---

## 🚀 Użycie / Usage

### Pierwsze Uruchomienie / First Run

1. **Wgraj firmware** na ESP32
2. **Otwórz Serial Monitor** (115200 baud) aby zobaczyć logi
3. ESP32 próbuje połączyć się z WiFi:
   - **Sukces**: Wyświetli adres IP
   - **Niepowodzenie**: Uruchomi tryb AP

4. **Otwórz przeglądarkę** i przejdź do:
   - Tryb WiFi: `http://[IP_ESP32]`
   - Tryb AP: `http://192.168.4.1`

### Interfejs Webowy / Web Interface

#### Dashboard
- Status systemu (WiFi, IP, Uptime, Pamięć)
- Szybkie akcje (skanowanie, odświeżanie)

#### Devices (Urządzenia)
- Lista wszystkich urządzeń Shelly
- Włączanie/wyłączanie
- Monitorowanie mocy
- Dodawanie nowych urządzeń
- Usuwanie urządzeń

#### Rules (Reguły)
- Definiowanie reguł automatyki
- Zarządzanie warunkami włączania/wyłączania
- Ustawianie progów mocy

#### Settings (Ustawienia)
- Konfiguracja WiFi
- Export/Import konfiguracji
- Aktualizacje firmware (OTA)

### Sterowanie przez API / API Control

Zobacz sekcję [API Endpoints](#-api-endpoints) poniżej.

---

## 📁 Struktura Projektu / Project Structure

```
Mietek-2-pistacja-esp/
├── src/                          # Kod źródłowy / Source code
│   ├── main.cpp                  # Główny plik aplikacji / Main application
│   ├── config.h                  # Konfiguracja globalna / Global configuration
│   ├── secrets.h                 # Dane uwierzytelniające / Credentials
│   ├── secrets_template.h        # Szablon secrets / Secrets template
│   ├── WiFiManager.cpp/h         # Zarządzanie WiFi / WiFi management
│   ├── Storage.cpp/h             # Przechowywanie danych / Data storage
│   ├── ShellyClient.cpp/h        # Komunikacja z Shelly / Shelly communication
│   ├── Discovery.cpp/h           # Wykrywanie urządzeń / Device discovery
│   ├── LogicEngine.cpp/h         # Silnik reguł / Rules engine
│   └── RESTClient.cpp/h          # Klient REST / REST client
│
├── data/                         # Pliki WWW / Web files
│   ├── index.html                # Interfejs webowy / Web interface
│   ├── style.css                 # Arkusz stylów / Stylesheet
│   ├── script.js                 # JavaScript
│   ├── example-config.json       # Przykładowa konfiguracja / Example config
│   └── example-devices.json      # Przykładowe urządzenia / Example devices
│
├── docs/                         # Dokumentacja / Documentation
│   ├── innyprogram-dozobaczenia  # Wersja referencyjna / Reference version
│   └── compile.log               # Log kompilacji / Compile log
│
├── examples/                     # Przykłady / Examples
│   └── simple-monolithic-version.cpp  # Prosta wersja monolityczna / Simple version
│
├── platformio.ini                # Konfiguracja PlatformIO
├── .gitignore                    # Pliki ignorowane przez Git
└── README.md                     # Ten plik / This file
```

---

## 🌐 API Endpoints

### System Status
```http
GET /api/status
```
Zwraca status systemu (WiFi, pamięć, uptime).

### WiFi Configuration
```http
POST /api/wifi/config
Content-Type: application/json

{
  "ssid": "MojaSiec",
  "password": "MojeHaslo"
}
```

### Get Devices List
```http
GET /api/devices
```
Zwraca listę wszystkich urządzeń.

### Add Device
```http
POST /api/device/add
Content-Type: application/json

{
  "mac": "AA:BB:CC:DD:EE:FF",
  "name": "Bojler"
}
```

### Control Device
```http
POST /api/device/control
Content-Type: application/json

{
  "deviceId": "shelly_AABBCCDDEEFF",
  "state": true
}
```
`state`: `true` = ON, `false` = OFF

### Delete Device
```http
POST /api/device/delete
Content-Type: application/json

{
  "id": "shelly_AABBCCDDEEFF"
}
```

### Start Discovery
```http
POST /api/discovery/scan
```
Rozpoczyna skanowanie sieci w poszukiwaniu urządzeń Shelly.

### Get Rules
```http
GET /api/rules
```
Zwraca listę reguł automatyki.

### Export Configuration
```http
GET /api/config/export
```
Eksportuje całą konfigurację (WiFi, urządzenia) do JSON.

### Import Configuration
```http
POST /api/config/import
Content-Type: application/json

{
  "devices": [...]
}
```

---

## 🔧 Rozwój / Development

### Kompilacja / Building

```bash
# Kompilacja bez wgrywania / Build without upload
pio run

# Wgranie firmware / Upload firmware
pio run --target upload

# Wgranie systemu plików (LittleFS) / Upload filesystem
pio run --target uploadfs

# Czyszczenie / Clean
pio run --target clean
```

### Monitorowanie / Monitoring

```bash
# Serial monitor
pio device monitor

# Serial monitor z filtrem wyjątków ESP32
pio device monitor --filter esp32_exception_decoder
```

### OTA Update

Po pierwszym wgraniu przez USB, możesz aktualizować przez WiFi:

```bash
# W platformio.ini ustaw:
upload_protocol = espota
upload_port = 192.168.1.XXX  # IP twojego ESP32
upload_flags = --auth=pistachio-ota
```

### Debugowanie / Debugging

Logi są dostępne na porcie szeregowym (115200 baud). Poziom debugowania można ustawić w `config.h`:

```cpp
#define DEBUG_ENABLE true
#define DEBUG_VERBOSE true  // Szczegółowe logi
```

---

## 📝 Notatki / Notes

### Wersja Prosta vs. Modularna / Simple vs. Modular

**Simple Version** (`examples/simple-monolithic-version.cpp`):
- Jeden plik
- Łatwiejsza do zrozumienia
- Idealna do nauki i testów
- **Potwierdzona jako działająca** - włączanie/wyłączanie urządzeń działa poprawnie

**Modular Version** (`src/`):
- Podzielona na moduły
- Łatwiejsza do rozbudowy
- Profesjonalna struktura
- Dodatkowe funkcje (Logic Engine, REST Backend)

### Kompatybilność / Compatibility

Projekt jest testowany z:
- **Shelly Gen4** (Plus 1PM, Plus 2PM, Pro 1PM, Pro 2PM)
- ESP32 DevKit v1
- ESP32-WROOM-32

### Bezpieczeństwo / Security

⚠️ **WAŻNE**: Plik `secrets.h` zawiera dane uwierzytelniające i **NIE POWINIEN** być commitowany do repozytorium. Jest on dodany do `.gitignore`.

---

## 🤝 Wkład / Contributing

Zapraszamy do zgłaszania problemów (Issues) i Pull Requestów!

---

## 📄 Licencja / License

Ten projekt jest dostępny na licencji MIT - szczegóły w pliku LICENSE.

---

## 👤 Autor / Author

**MrBorkim**

---

## 🙏 Podziękowania / Acknowledgments

- Społeczność ESP32
- Twórcy biblioteki ArduinoJson
- Zespół Shelly za doskonałe API

---

## ❓ FAQ

### Q: ESP32 nie łączy się z WiFi
**A:** Sprawdź:
1. Czy SSID i hasło w `secrets.h` są poprawne
2. Czy ESP32 jest w zasięgu sieci
3. Czy router WiFi działa na 2.4 GHz (ESP32 nie obsługuje 5 GHz)
4. Uruchom w trybie AP i skonfiguruj przez przeglądarkę

### Q: Nie mogę znaleźć urządzeń Shelly
**A:** Upewnij się, że:
1. Urządzenia Shelly są w tej samej sieci co ESP32
2. Urządzenia są włączone i skonfigurowane
3. Używasz urządzeń Shelly Gen4 (starsze wersje mogą wymagać modyfikacji API)
4. Sprawdź logi na Serial Monitor podczas skanowania

### Q: Jak zaktualizować firmware?
**A:** Możesz:
1. Użyć OTA przez interfejs webowy (Settings → OTA Update)
2. Wgrać przez USB używając PlatformIO: `pio run -t upload`
3. Wgrać przez ESP32 OTA: skonfiguruj `upload_protocol = espota` w `platformio.ini`

### Q: Jak zresetować konfigurację?
**A:**
1. Przez interfejs: Settings → Reset Configuration
2. Fizycznie: Skasuj partycję LittleFS i wgraj ponownie firmware
3. Kod: Wywołaj `Storage::format()` w setup()

---

## 📞 Wsparcie / Support

Jeśli masz pytania lub problemy:
1. Sprawdź FAQ powyżej
2. Przejrzyj [Issues](https://github.com/MrBorkim/Mietek-2-pistacja-esp/issues)
3. Utwórz nowy Issue z opisem problemu i logami

---

**Miłego użytkowania! / Enjoy!** 🥜✨
