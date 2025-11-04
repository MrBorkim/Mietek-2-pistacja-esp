/*
  ESP32 Device Manager — FULL (WiFi + Config + WebUI + Shelly mDNS/Scan Auto‑Discover)

  Funkcje:
  - Przechowuje ustawienia WiFi i listę urządzeń Shelly (id/rpc_id, mac, ip, name) w LittleFS:/config.json
  - Jeśli /config.json nie istnieje → tworzy automatycznie z domyślnym SSID/hasłem (edytuj w kodzie)
  - REST API: /config (GET/POST), /devices (GET/POST), /toggle (GET), /discover (GET)
  - Web UI pod "/": dodawanie/edycja urządzeń, włącz/wyłącz, przycisk „Auto‑discover” (mDNS→scan fallback)
  - Auto‑discover: próbuje mDNS _shelly._tcp.local, a gdy nic nie znajdzie robi szybki skan /24 z RPC probe

  Biblioteki: ArduinoJson, LittleFS (ESP32), ESPmDNS, WiFi, WebServer, HTTPClient
*/

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <vector>

#define CONFIG_PATH "/config.json"
#define LISTEN_PORT 80

WebServer server(LISTEN_PORT);

// ======= Ustawienia domyślne WiFi (edytuj) =======
// Pozostaw puste, aby wymusić tryb AP i konfigurację przez API/plik
const char* DEFAULT_WIFI_SSID = "";   // np. "MojaSiec"
const char* DEFAULT_WIFI_PASS = "";   // np. "MojeHaslo"

// ======= Struktury =======
struct Device {
  int id;       // używane też jako RPC id dla Switch.Set
  String name;
  String mac;
  String ip;
};

String wifi_ssid;
String wifi_pass;
std::vector<Device> devices;

// ======= FS =======
bool ensureFS() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return false;
  }
  return true;
}

bool writeDefaultConfig() {
  if (!ensureFS()) return false;
  if (LittleFS.exists(CONFIG_PATH)) return true;
  StaticJsonDocument<1024> doc;
  JsonObject w = doc.createNestedObject("wifi");
  w["ssid"] = String(DEFAULT_WIFI_SSID);
  w["pass"] = String(DEFAULT_WIFI_PASS);
  doc.createNestedArray("devices");
  File f = LittleFS.open(CONFIG_PATH, "w");
  if (!f) { Serial.println("Nie można utworzyć domyślnego config.json"); return false; }
  serializeJsonPretty(doc, f);
  f.close();
  Serial.println("Utworzono domyślny config.json");
  return true;
}

bool loadConfig() {
  devices.clear();
  if (!ensureFS()) return false;
  if (!LittleFS.exists(CONFIG_PATH)) {
    Serial.println("Brak config.json - tworzę domyślny plik...");
    if (!writeDefaultConfig()) return false;
  }
  File f = LittleFS.open(CONFIG_PATH, "r");
  if (!f) { Serial.println("Nie można otworzyć config.json"); return false; }
  size_t size = f.size();
  std::unique_ptr<char[]> buf(new char[size + 1]);
  f.readBytes(buf.get(), size);
  buf[size] = '\0';
  f.close();

  StaticJsonDocument<4096> doc;
  DeserializationError err = deserializeJson(doc, buf.get());
  if (err) { Serial.print("Błąd parsowania config.json: "); Serial.println(err.c_str()); return false; }

  if (doc.containsKey("wifi")) {
    const char* p1 = doc["wifi"]["ssid"] | "";
    const char* p2 = doc["wifi"]["pass"] | "";
    wifi_ssid = String(p1 ? p1 : "");
    wifi_pass = String(p2 ? p2 : "");
  }

  if (doc.containsKey("devices")) {
    JsonArray arr = doc["devices"].as<JsonArray>();
    for (JsonObject obj : arr) {
      Device d; d.id = obj["id"] | -1;
      const char* pn = obj["name"] | "";
      const char* pm = obj["mac"]  | "";
      const char* pi = obj["ip"]   | "";
      d.name = String(pn ? pn : "");
      d.mac  = String(pm ? pm : "");
      d.ip   = String(pi ? pi : "");
      if (d.id >= 0) devices.push_back(d);
    }
  }
  Serial.println("Wczytano konfigurację");
  return true;
}

bool saveConfig() {
  if (!ensureFS()) return false;
  StaticJsonDocument<4096> doc;
  JsonObject w = doc.createNestedObject("wifi");
  w["ssid"] = wifi_ssid.c_str();
  w["pass"] = wifi_pass.c_str();
  JsonArray arr = doc.createNestedArray("devices");
  for (auto &d : devices) {
    JsonObject obj = arr.createNestedObject();
    obj["id"] = d.id; obj["name"] = d.name.c_str(); obj["mac"] = d.mac.c_str(); obj["ip"] = d.ip.c_str();
  }
  File f = LittleFS.open(CONFIG_PATH, "w");
  if (!f) { Serial.println("Nie można zapisać config.json"); return false; }
  serializeJsonPretty(doc, f); f.close();
  Serial.println("Zapisano konfigurację");
  return true;
}

// ======= WiFi / AP =======
void startAP() {
  const char* apName = "ESP32-Config";
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apName);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP uruchomione: "); Serial.println(apName);
  Serial.print("AP IP: "); Serial.println(ip);
}

bool tryConnectWiFi() {
  if (wifi_ssid.length() == 0) return false;
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
  Serial.print("Łączenie z WiFi: "); Serial.println(wifi_ssid);
  unsigned long start = millis();
  while (millis() - start < 15000) {
    if (WiFi.status() == WL_CONNECTED) break;
    delay(250);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Połączono. IP: "); Serial.println(WiFi.localIP());
    return true;
  }
  Serial.println("Nie udało się połączyć z WiFi");
  return false;
}

// ======= Helpers: devices =======
void addOrUpdateDeviceByMac(const String &mac, const String &ip, const String &name, int rpc_id=-1) {
  for (auto &d : devices) {
    if (mac.length() && d.mac.length() && d.mac.equalsIgnoreCase(mac)) {
      d.ip = ip; if (name.length()) d.name = name; saveConfig(); return;
    }
    if (rpc_id >= 0 && d.id == rpc_id) {
      d.ip = ip; if (name.length()) d.name = name; saveConfig(); return;
    }
  }
  // dodaj nowe
  Device nd; nd.id = (rpc_id >= 0) ? rpc_id : 0;
  if (rpc_id < 0) {
    bool ok;
    do { ok = true; for (auto &x : devices) if (x.id == nd.id) { nd.id++; ok = false; break; } } while (!ok);
  }
  nd.mac = mac; nd.ip = ip; nd.name = name.length()? name : String("Shelly-") + (mac.length()? mac.substring(max(0, (int)mac.length()-4)) : ip);
  devices.push_back(nd);
  saveConfig();
}

// ======= Shelly RPC =======
bool sendShellyToggle(const String &deviceIp, int rpcId, bool on) {
  if (deviceIp.length() == 0) return false;
  String url = "http://" + deviceIp + "/rpc/Switch.Set?id=" + String(rpcId) + "&on=" + (on?"true":"false");
  Serial.print("Wysyłam: "); Serial.println(url);
  HTTPClient http; http.begin(url); int code = http.GET();
  if (code > 0) { String payload = http.getString(); Serial.printf("HTTP %d\n", code); }
  else { Serial.printf("Błąd HTTP: %d\n", code); }
  http.end();
  return (code >= 200 && code < 300);
}

// ======= Discover (mDNS + scan) =======
const unsigned long MDNS_TIMEOUT_MS = 3000;

bool probeShellyAtIp(const IPAddress &ip) {
  String url = String("http://") + ip.toString() + "/rpc/Shelly.GetDeviceInfo";
  HTTPClient http; http.begin(url); http.setTimeout(500);
  int code = http.GET();
  bool found = false;
  if (code >= 200 && code < 300) {
    String payload = http.getString();
    // bardzo prosty parsing mac i ewentualnego id
    int p = payload.indexOf("\"mac\""); String mac="";
    if (p>=0) { int q = payload.indexOf('"', p+6); int r = payload.indexOf('"', q+1); if (q>0 && r>q) mac = payload.substring(q+1, r); }
    int pi = payload.indexOf("\"id\""); int rpc_id = -1;
    if (pi>=0) { int col = payload.indexOf(':', pi); if (col>0) { int j=col+1; while(j<(int)payload.length() && (payload[j]==' '||payload[j]=='"')) j++; String num=""; while(j<(int)payload.length() && isDigit(payload[j])) { num += payload[j]; j++; } if (num.length()) rpc_id = num.toInt(); } }
    addOrUpdateDeviceByMac(mac, ip.toString(), String("Shelly-")+ip.toString(), rpc_id);
    found = true;
  }
  http.end();
  return found;
}

bool discoverShellyByMDNS() {
  Serial.println("Szukam Shelly przez mDNS...");
  int n = MDNS.queryService("_shelly", "tcp");
  if (n <= 0) { Serial.println("mDNS: brak wyników"); return false; }
  Serial.printf("mDNS: znaleziono %d usług", n);
  for (int i = 0; i < n; i++) {
    String host = MDNS.hostname(i);
    IPAddress ip;
    if (!WiFi.hostByName((host + String(".local")).c_str(), ip)) {
      Serial.printf("mDNS: nie mogę rozwiązać %s.local", host.c_str());
      continue;
    }
    Serial.printf("-> %s (%s)", host.c_str(), ip.toString().c_str());
    probeShellyAtIp(ip);
  }
  return true;
}

bool discoverShellyByScan() {
  if (WiFi.status() != WL_CONNECTED) return false;
  IPAddress local = WiFi.localIP();
  Serial.print("Skanuję podsieć /24 od: "); Serial.println(local);
  uint8_t base[4] = { local[0], local[1], local[2], 0 };
  int found = 0;
  for (int i=1;i<255;i++) {
    IPAddress ip(base[0], base[1], base[2], i);
    if (ip == local) continue;
    if (probeShellyAtIp(ip)) found++;
    delay(2); // delikatna pauza
  }
  Serial.printf("Scan: znaleziono %d kandydatów\n", found);
  return found>0;
}

// ======= Web UI (HTML) =======
const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="pl">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Device Manager</title>
  <style>
    :root{--bg:#0f172a;--card:#111827;--txt:#e5e7eb;--muted:#9ca3af;--acc:#22c55e;--acc2:#ef4444}
    *{box-sizing:border-box;font-family:system-ui,Segoe UI,Roboto,Helvetica,Arial}
    body{margin:0;background:linear-gradient(180deg,#0b1022,#0f172a);color:var(--txt)}
    header{padding:18px 16px;border-bottom:1px solid #1f2937;display:flex;gap:12px;align-items:center}
    header h1{margin:0;font-size:18px;font-weight:600}
    main{max-width:980px;margin:24px auto;padding:0 16px;display:grid;gap:16px}
    .card{background:var(--card);border:1px solid #1f2937;border-radius:14px;padding:16px}
    .row{display:flex;gap:8px;flex-wrap:wrap}
    input,button{padding:10px 12px;border-radius:10px;border:1px solid #374151;background:#0b1220;color:var(--txt)}
    button{cursor:pointer}
    button.primary{background:#14532d;border-color:#166534}
    button.danger{background:#3f1111;border-color:#7f1d1d}
    table{width:100%;border-collapse:collapse}
    th,td{padding:10px;border-bottom:1px solid #1f2937;text-align:left}
    th{color:var(--muted);font-weight:600}
    .pill{padding:4px 8px;border-radius:999px;font-size:12px;border:1px solid #334155;color:#93c5fd}
    .muted{color:var(--muted)}
    .ok{color:#86efac}
    .err{color:#fca5a5}
    code{background:#0b1220;border:1px solid #1f2937;border-radius:8px;padding:2px 6px}
  </style>
</head>
<body>
  <header>
    <h1>ESP32 Device Manager</h1>
    <span id="status" class="pill">status</span>
  </header>

  <main>
    <section class="card">
      <h3 style="margin-top:0">Ustawienia Wi‑Fi</h3>
      <div class="row">
        <input id="wifi_ssid" placeholder="SSID" style="min-width:220px">
        <input id="wifi_pass" placeholder="Hasło" type="password" style="min-width:220px">
        <button class="primary" onclick="saveWifi()">Zapisz Wi‑Fi</button>
      </div>
      <p class="muted" style="margin:8px 0 0">Po zapisaniu ESP spróbuje połączyć się z nową siecią. Jeśli się nie powiedzie, uruchomi własny AP <code>ESP32-Config</code>.</p>
    </section>

    <section class="card">
      <h3 style="margin-top:0">Dodaj / edytuj urządzenie</h3>
      <div class="row">
        <input id="dev_id" type="number" placeholder="id (rpc_id)" style="width:120px">
        <input id="dev_name" placeholder="nazwa" style="flex:1;min-width:160px">
        <input id="dev_mac" placeholder="MAC (opcjonalnie)" style="width:220px">
        <input id="dev_ip" placeholder="IP (np. 192.168.68.57)" style="width:220px">
        <button class="primary" onclick="saveDevice()">Zapisz</button>
      </div>
      <p class="muted" style="margin:8px 0 0">Uwaga: <code>id</code> to numer używany w <code>Switch.Set?id=ID</code> na Shelly.</p>
    </section>

    <section class="card">
      <div style="display:flex;justify-content:space-between;align-items:center;gap:12px">
        <h3 style="margin:0">Urządzenia</h3>
        <div class="row">
          <button onclick="loadDevices()">Odśwież</button>
          <button class="primary" onclick="discover()">Auto‑discover</button>
        </div>
      </div>
      <table>
        <thead>
          <tr><th>ID</th><th>Nazwa</th><th>MAC</th><th>IP</th><th>Sterowanie</th><th>Usuń</th></tr>
        </thead>
        <tbody id="tbody"></tbody>
      </table>
      <div id="msg" class="muted" style="margin-top:8px"></div>
    </section>
  </main>

<script>
const S = sel => document.querySelector(sel);
const tbody = S('#tbody');
const msg = S('#msg');
const statusEl = S('#status');

async function loadDevices(){
  try{
    statusEl.textContent = 'Ładowanie...';
    const r = await fetch('/devices');
    if(!r.ok) throw new Error('HTTP '+r.status);
    const arr = await r.json();
    tbody.innerHTML = '';
    if(!Array.isArray(arr) || arr.length===0){
      tbody.innerHTML = '<tr><td colspan="6" class="muted">Brak urządzeń — dodaj powyżej lub użyj Auto‑discover.</td></tr>';
    } else {
      for(const d of arr){
        const tr = document.createElement('tr');
        tr.innerHTML = `
          <td>${d.id ?? ''}</td>
          <td>${d.name ?? ''}</td>
          <td>${d.mac ?? ''}</td>
          <td>${d.ip ?? ''}</td>
          <td class="row">
            <button onclick="toggle(${Number(d.id)}, true)">WŁĄCZ</button>
            <button class="danger" onclick="toggle(${Number(d.id)}, false)">WYŁĄCZ</button>
          </td>
          <td><button class="danger" onclick="delDevice(${Number(d.id)})">Usuń</button></td>`;
        tbody.appendChild(tr);
      }
    }
    statusEl.textContent = 'OK'; statusEl.className='pill ok';
  }catch(e){
    statusEl.textContent = 'Błąd'; statusEl.className='pill err';
    msg.textContent = 'Nie udało się pobrać listy urządzeń: '+e.message;
  }
}

async function saveDevice(){
  const id = Number(S('#dev_id').value);
  const name = S('#dev_name').value.trim();
  const mac = S('#dev_mac').value.trim();
  const ip = S('#dev_ip').value.trim();
  if(isNaN(id) || id < 0){ msg.textContent='Podaj dodatnie id.'; return; }
  try{
    const r = await fetch('/devices',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({id,name,mac,ip})});
    if(!r.ok) throw new Error('HTTP '+r.status);
    msg.textContent = 'Zapisano urządzenie.';
    loadDevices();
  }catch(e){ msg.textContent='Błąd zapisu urządzenia: '+e.message; }
}

async function delDevice(id){
  if(!confirm('Usunąć urządzenie o id='+id+'?')) return;
  try{
    const r = await fetch('/devices/delete',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({id})});
    if(!r.ok) throw new Error('HTTP '+r.status);
    msg.textContent = 'Usunięto urządzenie id='+id;
    loadDevices();
  }catch(e){ msg.textContent='Błąd usuwania: '+e.message; }
}

async function toggle(id, on){
  try{
    const r = await fetch(`/toggle?id=${encodeURIComponent(id)}&on=${on?'true':'false'}`);
    if(!r.ok) throw new Error('HTTP '+r.status);
    msg.textContent = `Wysłano: id=${id}, on=${on}`;
  }catch(e){ msg.textContent = 'Błąd wysłania polecenia: '+e.message; }
}

async function discover(){
  msg.textContent = 'Skanowanie... (najpierw mDNS, potem scan)';
  statusEl.textContent = 'Skanowanie...'; statusEl.className='pill';
  try{
    const r = await fetch('/discover');
    if(!r.ok) throw new Error('HTTP '+r.status);
    const out = await r.json();
    msg.textContent = `Znaleziono przez mDNS: ${out.mdns_found}, przez scan: ${out.scan_found}`;
    statusEl.textContent = 'OK'; statusEl.className='pill ok';
    loadDevices();
  }catch(e){
    statusEl.textContent = 'Błąd'; statusEl.className='pill err';
    msg.textContent = 'Discover error: '+e.message;
  }
}

async function saveWifi(){
  const ssid = S('#wifi_ssid').value.trim();
  const pass = S('#wifi_pass').value;
  if(!ssid){ msg.textContent='Podaj SSID.'; return; }
  try{
    statusEl.textContent='Zapisywanie Wi‑Fi...';
    const r = await fetch('/wifi',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ssid,pass})});
    if(!r.ok) throw new Error('HTTP '+r.status);
    const out = await r.json();
    msg.textContent = out.ok ? ('Zapisano Wi‑Fi. '+ (out.connected?('Połączono: '+out.ip):'Nie udało się połączyć — uruchomiono AP.')) : 'Błąd zapisu.';
    statusEl.textContent = out.connected ? 'OK' : 'AP';
    statusEl.className = 'pill ' + (out.connected?'ok':'');
  }catch(e){ msg.textContent='Błąd Wi‑Fi: '+e.message; statusEl.textContent='Błąd'; statusEl.className='pill err'; }
}

loadDevices();
</script>
</body>
</html>
)HTML";

// ======= REST Handlers =======
void handleIndex(){ server.send(200, "text/html; charset=utf-8", INDEX_HTML); }

void handleGetConfig() {
  if (!ensureFS()) { server.send(500, "application/json", "{\"error\":\"FS mount failed\"}"); return; }
  if (!LittleFS.exists(CONFIG_PATH)) { server.send(404, "application/json", "{\"error\":\"config not found\"}"); return; }
  File f = LittleFS.open(CONFIG_PATH, "r"); String s; while (f.available()) s += char(f.read()); f.close();
  server.send(200, "application/json", s);
}

void handlePostConfig() {
  if (server.method() != HTTP_POST) { server.send(405); return; }
  if (!server.hasArg("plain")) { server.send(400, "application/json", "{\"error\":\"no body\"}"); return; }
  String body = server.arg("plain");
  StaticJsonDocument<4096> doc; if (deserializeJson(doc, body)) { server.send(400, "application/json", "{\"error\":\"invalid json\"}"); return; }
  if (doc.containsKey("wifi")) {
    const char* p1 = doc["wifi"]["ssid"] | ""; const char* p2 = doc["wifi"]["pass"] | "";
    wifi_ssid = String(p1 ? p1 : ""); wifi_pass = String(p2 ? p2 : "");
  }
  devices.clear();
  if (doc.containsKey("devices")) {
    JsonArray arr = doc["devices"].as<JsonArray>();
    for (JsonObject obj : arr) {
      Device d; d.id = obj["id"] | -1;
      const char* pn = obj["name"] | ""; const char* pm = obj["mac"] | ""; const char* pi = obj["ip"] | "";
      d.name = String(pn?pn:""); d.mac = String(pm?pm:""); d.ip = String(pi?pi:"");
      if (d.id >= 0) devices.push_back(d);
    }
  }
  if (saveConfig()) server.send(200, "application/json", "{\"ok\":true}");
  else server.send(500, "application/json", "{\"ok\":false}");
}

void handleGetDevices() {
  StaticJsonDocument<4096> doc; JsonArray arr = doc.to<JsonArray>();
  for (auto &d : devices) { JsonObject o = arr.createNestedObject(); o["id"]=d.id; o["name"]=d.name; o["mac"]=d.mac; o["ip"]=d.ip; }
  String out; serializeJsonPretty(arr, out); server.send(200, "application/json", out);
}

void handlePostDevice() {
  if (server.method() != HTTP_POST) { server.send(405); return; }
  if (!server.hasArg("plain")) { server.send(400, "application/json", "{\"error\":\"no body\"}"); return; }
  String body = server.arg("plain"); StaticJsonDocument<1024> doc;
  if (deserializeJson(doc, body)) { server.send(400, "application/json", "{\"error\":\"invalid json\"}"); return; }
  Device d; d.id = doc["id"] | -1; const char* pn = doc["name"] | ""; const char* pm = doc["mac"] | ""; const char* pi = doc["ip"] | "";
  d.name = String(pn?pn:""); d.mac = String(pm?pm:""); d.ip = String(pi?pi:"");
  if (d.id < 0) { server.send(400, "application/json", "{\"error\":\"id missing or negative\"}"); return; }
  bool found=false; for (auto &e : devices) if (e.id==d.id) { e=d; found=true; break; }
  if (!found) devices.push_back(d);
  if (saveConfig()) server.send(200, "application/json", "{\"ok\":true}");
  else server.send(500, "application/json", "{\"ok\":false}");
}

void handleToggle() {
  if (!server.hasArg("id")) { server.send(400, "application/json", "{\"error\":\"id missing\"}"); return; }
  int id = server.arg("id").toInt(); bool on=false;
  if (server.hasArg("on")) { String v = server.arg("on"); on = (v=="1" || v.equalsIgnoreCase("true")); }
  for (auto &d : devices) if (d.id==id) {
    bool ok = sendShellyToggle(d.ip, id, on);
    if (ok) server.send(200, "application/json", "{\"ok\":true}");
    else server.send(502, "application/json", "{\"ok\":false,\"note\":\"http request failed\"}");
    return;
  }
  server.send(404, "application/json", "{\"error\":\"device not found\"}");
}

void handleDiscover() {
  bool md = false, sc = false;
  // upewnij się, że mamy responder (często wymagane zanim queryService zadziała stabilnie)
  if (WiFi.status()==WL_CONNECTED) { MDNS.begin("esp32-device"); }
  md = discoverShellyByMDNS();
  if (!md) sc = discoverShellyByScan(); else sc = false; // skan tylko jeśli mDNS nic nie znalazł
  StaticJsonDocument<256> doc; doc["mdns_found"] = md; doc["scan_found"] = sc; String out; serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void setupRoutes() {
  server.on("/", HTTP_GET, handleIndex);
  server.on("/config", HTTP_GET, handleGetConfig);
  server.on("/config", HTTP_POST, handlePostConfig);
  server.on("/devices", HTTP_GET, handleGetDevices);
  server.on("/devices", HTTP_POST, handlePostDevice);
  server.on("/toggle", HTTP_GET, handleToggle);
  server.on("/discover", HTTP_GET, handleDiscover);
  server.onNotFound([](){ server.send(404, "text/plain", "Not found"); });
  server.begin();
}

void setup() {
  Serial.begin(115200); delay(300);
  Serial.println("--- ESP32 Device Manager — FULL ---");

  if (!ensureFS()) { Serial.println("LittleFS błąd. Uruchamiam AP."); startAP(); }
  if (!loadConfig()) { Serial.println("Błąd wczytywania konfiguracji. Tworzę AP."); startAP(); }

  // wstaw domyślne WiFi z kodu, jeśli config jest pusty i DEFAULT_* podane
  if (wifi_ssid.length()==0 && String(DEFAULT_WIFI_SSID).length()>0) {
    wifi_ssid = String(DEFAULT_WIFI_SSID); wifi_pass = String(DEFAULT_WIFI_PASS);
    Serial.println("Ustawiono WiFi z DEFAULT_*, zapisuję do config.json"); saveConfig();
  }

  bool ok = tryConnectWiFi();
  if (ok) {
    // start mDNS responder (własny hostname)
    if (!MDNS.begin("esp32-device")) {
      Serial.println("MDNS.begin() nie powiodło się (niekrytyczne)");
    } else {
      Serial.println("mDNS responder aktywny: esp32-device.local");
    }
  } else {
    startAP();
  }

  setupRoutes();
  Serial.println("HTTP server uruchomiony");
}

void loop() {
  server.handleClient();
  delay(2);
}
