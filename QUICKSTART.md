# 🚀 Szybki Start / Quick Start

## Dla niecierpliwych / For the impatient

### 1. Sklonuj projekt / Clone project
```bash
git clone https://github.com/MrBorkim/Mietek-2-pistacja-esp.git
cd Mietek-2-pistacja-esp
```

### 2. Zainstaluj PlatformIO / Install PlatformIO
```bash
pip install platformio
```

### 3. Skonfiguruj WiFi / Configure WiFi
Edytuj `src/secrets.h`:
```cpp
#define DEFAULT_WIFI_SSID "Twoja_Siec"
#define DEFAULT_WIFI_PASSWORD "TwojeHaslo"
```

**LUB / OR** zostaw puste i użyj trybu AP:
- Po uruchomieniu ESP32 połącz się z siecią WiFi: `Pistachio-Config-XXXXXX`
- Hasło: `pistachio123`
- Otwórz: `http://192.168.4.1`

### 4. Wgraj na ESP32 / Upload to ESP32
```bash
pio run --target upload
```

### 5. Otwórz interfejs / Open interface
- Sprawdź Serial Monitor aby znaleźć IP
- Otwórz w przeglądarce: `http://[IP_ESP32]`

### 6. Dodaj urządzenia Shelly / Add Shelly devices
**Automatycznie / Auto:**
- Kliknij "Discover Devices" w interfejsie

**Ręcznie / Manual:**
- Devices → Add Device Manually
- Wpisz MAC i nazwę urządzenia

---

## 🎯 Pierwszy Test / First Test

Po dodaniu urządzenia Shelly:

1. Przejdź do zakładki **Devices**
2. Znajdź swoje urządzenie
3. Kliknij przycisk **WŁĄCZ** lub **WYŁĄCZ**
4. Urządzenie powinno się włączyć/wyłączyć! ✨

**Uwaga:** To co napisałeś "jak dodałem urządzenie ręcznie to działało włącz i wyłącz" - właśnie tak działa! 🎉

---

## 📋 Checklist

- [ ] PlatformIO zainstalowane
- [ ] Projekt sklonowany
- [ ] WiFi skonfigurowane w `src/secrets.h`
- [ ] Firmware wgrane na ESP32
- [ ] Interfejs otwarty w przeglądarce
- [ ] Urządzenie Shelly dodane (auto lub ręcznie)
- [ ] Test włącz/wyłącz działa ✅

---

## ❓ Problemy? / Issues?

### Serial Monitor pokazuje błąd WiFi
```bash
# Zobacz logi
pio device monitor

# Sprawdź SSID i hasło w src/secrets.h
```

### Nie widzę interfejsu
```bash
# Sprawdź IP w Serial Monitor
# ESP32 wyświetla IP po połączeniu:
# "Connected to WiFi"
# "IP Address: 192.168.X.XXX"
```

### Urządzenia nie są wykrywane
```bash
# Upewnij się że:
# 1. Shelly i ESP32 są w tej samej sieci
# 2. Urządzenia Shelly są włączone
# 3. To są urządzenia Shelly Gen4
```

---

## 🎓 Więcej Info / More Info

Zobacz pełną dokumentację w [README.md](README.md)

---

**Powodzenia! / Good luck!** 🥜
