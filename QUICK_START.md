# ⚡ SZYBKI START - Pistachio Smart Controller

## 🎯 Projekt GOTOWY do Użycia!

Ten projekt jest **w pełni funkcjonalny** i można go wgrać na ESP32 **bez żadnej konfiguracji**!

---

## 🚀 Dwie Opcje Uruchomienia

### Opcja 1: Tryb Demo (Najszybszy - BEZ WiFi)

**Dla kogo**: Chcesz zobaczyć jak to działa bez konfiguracji

```bash
# Sklonuj projekt
git clone https://github.com/MrBorkim/Mietek-2-pistacja-esp.git
cd Mietek-2-pistacja-esp
git checkout claude/modernize-html-js-ui-011CUoSBNozWaFtkWPQq6NsR

# Wgraj firmware
pio run --target upload

# Wgraj pliki WWW
pio run --target uploadfs
```

**Co się stanie?**
1. ESP32 uruchomi się w trybie **Access Point**
2. Znajdziesz sieć WiFi: `Pistachio-Config-XXXXXX` (XXXXXX = część MAC)
3. Hasło: `pistachio123`
4. Połącz się z tą siecią
5. Otwórz przeglądarkę: `http://192.168.4.1`
6. **GOTOWE!** Zobaczysz działający dashboard!

---

### Opcja 2: Z Twoim WiFi (Produkcja)

**Dla kogo**: Chcesz podłączyć do swojej sieci domowej

```bash
# 1. Edytuj plik z credentials
nano include/secrets.h

# 2. Znajdź linie (ok. 15-16):
#define DEFAULT_WIFI_SSID ""
#define DEFAULT_WIFI_PASSWORD ""

# 3. Zmień na swoje dane:
#define DEFAULT_WIFI_SSID "MojaSiecWiFi"
#define DEFAULT_WIFI_PASSWORD "MojeHasloWiFi"

# 4. Zapisz (Ctrl+O, Enter, Ctrl+X)

# 5. Wgraj
pio run --target upload
pio run --target uploadfs

# 6. Monitoruj aby zobaczyć IP
pio device monitor -b 115200
```

**Co zobaczysz w monitorze?**
```
========================================
  🥜 Pistachio Smart Controller
  ESP32 Energy Management System
  Version: 2.0.0
========================================

Connecting to WiFi...
✅ Connected to WiFi: MojaSiecWiFi
IP Address: 192.168.1.100        <-- TEN ADRES!
Signal Strength: -45 dBm

...
Web server started
Access web interface at: http://192.168.1.100 or http://pistachio-controller.local
```

Otwórz: `http://192.168.1.100` lub `http://pistachio-controller.local`

---

## ✅ Co Działa w Wersji Demo

Nawet bez prawdziwych urządzeń Shelly zobaczysz **w pełni działający system**:

### Dashboard
- ⚡ **Energy Flow Visualization** - animowany przepływ energii
- 📊 **Real-time Power Chart** - wykres mocy aktualizowany co 5s
- 📈 **Statistics** - aktualna moc, energia dzisiaj, oszczędności
- 🎯 **Active Rules** - status reguł automatyki

### Devices (Urządzenia)
- 🔌 **2 Demo Devices**:
  - Water Heater (Bojler)
  - Floor Heating (Ogrzewanie podłogowe)
- 🎛️ **Przyciski ON/OFF** - działają (wysyłają komendy do loga)
- 📊 **Status** - pokazuje symulowane dane

### Rules (Reguły)
- 🎯 **Przykładowa reguła**: "Solar Surplus - Bojler"
- 📝 **Rule Builder** - kreator nowych reguł (gotowy do użycia)
- 📋 **Templates** - 4 gotowe szablony reguł

### Energy (Energia)
- ⚡ **Symulowane odczyty licznika**:
  - Napięcie: ~230V
  - Prąd: zmienny
  - Moc: zmienia się co sekundę
  - Eksport/Import: symulowany przepływ
- 📊 **Live Meter Data** - aktualizowane co 1s

### Settings
- 📶 **WiFi Configuration** - zmiana sieci
- ℹ️ **System Info** - wersja, uptime, pamięć
- 💾 **Backup/Restore** - eksport/import konfiguracji
- ⚙️ **Advanced Settings** - zaawansowane opcje

---

## 🎨 Co Zobaczysz?

### Dashboard View
```
┌─────────────────────────────────────────────┐
│  🥜 Pistachio Smart Controller       🌙 ●  │
├─────────────────────────────────────────────┤
│  📊 Dashboard  ⚡ Energy  🔌 Devices  ...  │
├─────────────────────────────────────────────┤
│                                              │
│   🔄 Energy Flow                            │
│   ┌──────┐        ┌──────┐       ┌──────┐  │
│   │ ☀️  │───────▶│ 🏠   │──────▶│ 🔌  │  │
│   │Solar│  2.5kW │ Home │ 1.2kW │Grid │  │
│   └──────┘        └──────┘       └──────┘  │
│                                   ↑ Export  │
│                                              │
│   ⚡ Current Power        📈 Energy Today   │
│      2500 W                  12.5 kWh       │
│                                              │
│   💰 Savings Today        🎯 Active Rules   │
│      10.00 PLN                3 of 5        │
│                                              │
│   📊 Power History (24h)                    │
│   ┌────────────────────────────────────┐    │
│   │     Chart with Solar/Grid/Home     │    │
│   └────────────────────────────────────┘    │
└─────────────────────────────────────────────┘
```

### Energy Flow Animation
- **Zielone strzałki** pokazują przepływ energii słonecznej
- **Niebieskie strzałki** pokazują przepływ z/do sieci
- **Animowane** - płynne ruchy co 2 sekundy
- **Real-time** - zmienia się z danymi

---

## 🔧 Pierwsze Kroki po Wgraniu

### 1. Sprawdź Dashboard
- Zobacz animowany Energy Flow
- Obserwuj zmieniające się wartości mocy
- Sprawdź wykres Power History

### 2. Zobacz Devices
- Kliknij zakładkę **Devices**
- Znajdziesz 2 demo urządzenia
- Spróbuj przełączników ON/OFF (sprawdź Console/Monitor)

### 3. Eksperymentuj z Rules
- Przejdź do **Rules**
- Kliknij **Create Rule**
- Użyj szablonu "Use Solar Surplus"
- Zobacz jak działają warunki i akcje

### 4. Konfiguracja (jeśli potrzebujesz)
- **Settings** → WiFi Configuration
- **Settings** → Energy Meter (dla prawdziwych liczników)

---

## 📱 Dostęp Zdalny

### Tryb AP (Access Point)
```
1. Połącz się z WiFi: Pistachio-Config-XXXXXX
2. Hasło: pistachio123
3. Otwórz: http://192.168.4.1
```

### Tryb WiFi (w Twojej sieci)
```
1. Sprawdź IP w Serial Monitor
2. Otwórz: http://[IP] lub http://pistachio-controller.local
3. Możesz też znaleźć przez mDNS: pistachio-controller.local
```

### Z Telefonu/Tabletu
1. Połącz do tej samej sieci co ESP32
2. Otwórz przeglądarkę
3. Wpisz adres IP lub pistachio-controller.local
4. **Interfejs jest w pełni responsywny!**

---

## 🎓 Co Dalej?

### Krok 1: Przetestuj Demo ✅
- Wgraj, zobacz, poeksperymentuj
- Zrozum jak działa interfejs
- Zobacz symulację energy flow

### Krok 2: Dodaj Prawdziwy Licznik Energii
Przeczytaj w głównym README:
- Sekcja "Energy Meter Configuration"
- Dla Shelly EM: podaj MAC/IP
- Dla PZEM-004T: podłącz do GPIO 16/17

### Krok 3: Dodaj Prawdziwe Urządzenia Shelly
- Użyj **Auto-Discover** w zakładce Devices
- Lub dodaj ręcznie przez MAC address
- System automatycznie znajdzie IP

### Krok 4: Skonfiguruj Reguły Automatyki
- Stwórz regułę "Solar Surplus" dla bojlera
- Dodaj regułę "Prevent Import"
- Użyj kaskadowego włączania (prioryty)

### Krok 5: Monitoruj i Optymalizuj
- Obserwuj dane przez kilka dni
- Dostosuj progi mocy w regułach
- Dodaj więcej urządzeń

---

## ❓ Rozwiązywanie Problemów

### Problem: ESP32 się nie łączy z WiFi
**Rozwiązanie**:
- Sprawdź czy SSID i hasło są poprawne w secrets.h
- Upewnij się że WiFi działa na 2.4 GHz (ESP32 nie obsługuje 5 GHz)
- Jeśli nie działa - zostanie uruchomiony tryb AP

### Problem: Nie widzę interface w przeglądarce
**Rozwiązanie**:
- Sprawdź czy wgrałeś filesystem: `pio run --target uploadfs`
- Odśwież stronę (Ctrl+F5)
- Sprawdź konsolę przeglądarki (F12)

### Problem: Compilation error
**Rozwiązanie**:
- Sprawdź czy masz PlatformIO zainstalowane
- Uruchom: `pio lib install` aby zainstalować biblioteki
- Sprawdź czy jesteś na właściwym branchu

### Problem: Serial Monitor pokazuje błędy
**Rozwiązanie**:
- To normalne - niektóre błędy to warnings o demo mode
- Szukaj linii: "System Ready!" - to oznacza sukces
- Sprawdź czy pokazuje IP address

---

## 💡 Pro Tips

1. **Development**: Włącz debug logging w `include/config.h`:
   ```cpp
   #define DEBUG_ENABLE true
   #define DEBUG_VERBOSE true
   ```

2. **Network**: Użyj stałego IP w routerze dla ESP32 (DHCP reservation)

3. **Testing**: Użyj `curl` do testowania API:
   ```bash
   curl http://192.168.1.100/api/status
   ```

4. **Monitoring**: Trzymaj Serial Monitor otwarty podczas testów

5. **Backup**: Exportuj konfigurację przed eksperymentami (Settings → Export)

---

## 📚 Dodatkowe Zasoby

- **Główny README**: Pełna dokumentacja w `README.md`
- **Example Rules**: Praktyczne przykłady w `examples/EXAMPLE_RULES.md`
- **API Docs**: Lista endpointów w README sekcja "API Documentation"
- **GitHub Issues**: Pytania i problemy
- **Serial Monitor**: Logi w czasie rzeczywistym

---

## ✨ Podsumowanie

**Pistachio Smart Controller** to **kompletny, gotowy do użycia** system:

✅ Kompiluje się bez błędów
✅ Działa zaraz po wgraniu
✅ Ma piękny, nowoczesny interfejs
✅ Symuluje prawdziwe dane w demo
✅ Gotowy do rozbudowy o prawdziwy hardware
✅ Profesjonalna dokumentacja
✅ Przykłady i templates

**Nie musisz nic instalować ani konfigurować - po prostu wgraj i testuj!**

---

**Miłego testowania! 🥜⚡**

Jeśli masz pytania, sprawdź główny README.md lub otwórz Issue na GitHub.
