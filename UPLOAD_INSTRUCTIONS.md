# 📤 Instrukcje Wgrywania Firmware i Plików Web

## ⚠️ WAŻNE: Musisz wgrać DWA razy!

ESP32 wymaga wgrania **dwóch oddzielnych części**:
1. **Kod programu** (firmware) - plik `.bin`
2. **Pliki web** (HTML/CSS/JS) - system plików LittleFS

## 🔧 Metoda 1: PlatformIO (Zalecana)

### Krok 1: Wgraj Pliki Web do LittleFS

```bash
# W terminalu VSCode lub w folderze projektu:
pio run --target uploadfs
```

**Co robi to polecenie:**
- Bierze wszystkie pliki z folderu `data/`
- Pakuje je do obrazu LittleFS
- Wgrywa do partycji filesystem na ESP32
- Pliki które zostaną wgrane:
  - `index.html` (32 KB)
  - `style.css` (40 KB)
  - `script.js` (12 KB)
  - `script_wifi.js` (8 KB)

**Oczekiwany output:**
```
Building FS image from 'data' directory to .pio/build/esp32dev/littlefs.bin
Looking for upload port...
Auto-detected: /dev/cu.usbserial-110
Uploading .pio/build/esp32dev/littlefs.bin
Writing at 0x00290000... (100 %)
Wrote 1536000 bytes at 0x00290000 in 135.8 seconds
Hard resetting via RTS pin...
```

### Krok 2: Wgraj Kod Programu (Firmware)

```bash
# W terminalu VSCode:
pio run --target upload
```

**Co robi to polecenie:**
- Kompiluje kod C++ z `src/main.cpp`
- Wgrywa firmware do ESP32
- Uruchamia ponownie urządzenie

**Oczekiwany output:**
```
RAM:   [=         ]  14.2% (used 46484 bytes from 327680 bytes)
Flash: [======    ]  58.7% (used 768941 bytes from 1310720 bytes)
Uploading .pio/build/esp32dev/firmware.bin
Writing at 0x00010000... (100 %)
Wrote 768944 bytes at 0x00010000 in 68.2 seconds
Hard resetting via RTS pin...
```

### Krok 3: Sprawdź Monitor Serialny

```bash
pio device monitor
```

**Co powinieneś zobaczyć:**
```
[SYSTEM] Initializing Pistachio Smart Controller v2.0.0
[SYSTEM] Initializing LittleFS...
[SYSTEM] LittleFS mounted successfully
[SYSTEM] Storage: 1472 KB total, 92 KB used, 1380 KB free
[SYSTEM] Initializing WiFi...
[WiFi] No WiFi configuration found - starting AP mode
╔════════════════════════════════════════════╗
║    🥜 PISTACHIO - ACCESS POINT MODE        ║
╠════════════════════════════════════════════╣
║  SSID:     Pistachio-Config-XXXX           ║
║  Password: pistachio123                     ║
║  IP:       192.168.4.1                      ║
╠════════════════════════════════════════════╣
║  1. Connect to this WiFi network          ║
║  2. Open browser: http://192.168.4.1      ║
║  3. Configure your WiFi in Settings tab   ║
╚════════════════════════════════════════════╝
[SYSTEM] Web server started
[SYSTEM] Access web interface at: http://192.168.4.1
```

## 🔧 Metoda 2: Esptool (Zaawansowana)

Jeśli PlatformIO nie działa, możesz użyć esptool:

### Krok 1: Zainstaluj esptool

```bash
pip install esptool
```

### Krok 2: Zbuduj Obrazy

```bash
# Zbuduj firmware
pio run

# Zbuduj filesystem
pio run --target buildfs
```

### Krok 3: Wgraj Ręcznie

```bash
# Wgraj filesystem
esptool.py --chip esp32 --port /dev/cu.usbserial-110 --baud 460800 \
  write_flash 0x290000 .pio/build/esp32dev/littlefs.bin

# Wgraj firmware
esptool.py --chip esp32 --port /dev/cu.usbserial-110 --baud 460800 \
  write_flash 0x10000 .pio/build/esp32dev/firmware.bin
```

## 🐛 Rozwiązywanie Problemów

### Problem: "no permits for creation" w monitorze

```
[E][vfs_api.cpp:105] open(): /littlefs/index.html.gz does not exist
```

**Przyczyna:** Pliki web nie zostały wgrane do LittleFS!

**Rozwiązanie:**
```bash
pio run --target uploadfs  # NAJPIERW wgraj pliki!
pio run --target upload     # POTEM wgraj firmware
```

### Problem: "Port /dev/cu.usbserial-110 not found"

**Rozwiązanie:**
```bash
# Znajdź właściwy port:
pio device list

# Zaktualizuj platformio.ini:
upload_port = /dev/cu.usbserial-TWÓJ_PORT
```

### Problem: Strona się ładuje ale nie działa (brak urządzeń)

**Sprawdź console w przeglądarce:**
- Naciśnij F12
- Przejdź do zakładki Console
- Sprawdź czy są błędy JavaScript

**Typowe błędy:**
```
Failed to fetch /api/status
Failed to fetch /api/devices
```

**Rozwiązanie:** Sprawdź monitor serialny czy API odpowiada:
```bash
pio device monitor
# Powinieneś widzieć: GET /api/status → 200 OK
```

### Problem: ESP32 non-stop się restartuje

**Sprawdź monitor serialny:**
```bash
pio device monitor
```

**Szukaj:**
- Stack traces
- "Guru Meditation Error"
- Memory allocation failures

**Rozwiązanie:** Może być za mało pamięci - sprawdź czy pliki w `data/` nie są za duże.

## ✅ Weryfikacja Poprawnego Wgrania

### 1. Monitor Serialny Pokazuje:
```
✅ LittleFS mounted successfully
✅ Storage: 1472 KB total, 92 KB used
✅ Web server started
✅ Access web interface at: http://192.168.4.1
```

### 2. Przeglądarka Pokazuje:
```
✅ Ładna strona z logo Pistachio 🥜
✅ 5 zakładek: Dashboard, Energy, Devices, Rules, Settings
✅ Status: "AP Mode - Not connected" (przy pierwszym uruchomieniu)
✅ Brak błędów w Console (F12)
```

### 3. API Działa:
```
# W przeglądarce wejdź na:
http://192.168.4.1/api/status

# Powinieneś zobaczyć JSON:
{
  "version": "2.0.0",
  "uptime": 45,
  "wifi": {
    "connected": false,
    "ap_mode": true,
    "ip": "192.168.4.1"
  }
}
```

## 📋 Kolejność Kroków (Podsumowanie)

```bash
# 1. Zbuduj i wgraj filesystem
pio run --target uploadfs

# 2. Zbuduj i wgraj firmware
pio run --target upload

# 3. Sprawdź monitor
pio device monitor

# 4. Otwórz przeglądarkę
# - Połącz się z WiFi: Pistachio-Config-XXXX (hasło: pistachio123)
# - Wejdź na: http://192.168.4.1
# - Ciesz się działającym interfejsem!
```

## 🆘 Pomoc

Jeśli nadal masz problemy:

1. **Sprawdź wersję PlatformIO:**
```bash
pio --version
# Powinna być >= 6.0.0
```

2. **Wyczyść i przebuduj:**
```bash
pio run --target clean
pio run --target uploadfs
pio run --target upload
```

3. **Sprawdź czy pliki są w data/:**
```bash
ls -lh data/
# Powinny być: index.html, style.css, script.js, script_wifi.js
```

4. **Sprawdź rozmiary partycji:**
```bash
pio run
# RAM i Flash usage powinny być < 80%
```

## 📊 Rozmiary Plików (dla referencji)

```
data/index.html      : 32 KB
data/style.css       : 40 KB
data/script.js       : 12 KB
data/script_wifi.js  : 8 KB
────────────────────────────
TOTAL                : ~92 KB

Partycja LittleFS    : 1472 KB (1.5 MB)
Wykorzystanie        : ~6%
Wolne                : 1380 KB
```

Masz wystarczająco miejsca na dodatkowe pliki (logo, ikony, itp)!
