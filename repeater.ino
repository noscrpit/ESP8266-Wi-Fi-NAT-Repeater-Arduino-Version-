// ======================================================
// CZĘŚĆ 1: BIBLIOTEKI, ZMIENNE I OBSŁUGA PAMIĘCI FLASH
// ======================================================
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <lwip/napt.h>
#include <LittleFS.h>

const char* CONFIG_SSID = "MyAP_Arduino";
ESP8266WebServer server(80);

unsigned long ostatniaKontrola = 0;
const unsigned long interwalKontroli = 10000; 
unsigned long czasBezNetu = 0;

void zapiszSiec(String s, String p) {
  File f = LittleFS.open("/net.txt", "w");
  if (f) { f.println(s); f.println(p); f.close(); }
}

bool wczytajSiec(String &s, String &p) {
  if (!LittleFS.exists("/net.txt")) return false;
  File f = LittleFS.open("/net.txt", "r");
  if (!f) return false;
  s = f.readStringUntil('\n'); p = f.readStringUntil('\n');
  s.trim(); p.trim(); f.close();
  return (s.length() > 0);
}
// ======================================================
// CZĘŚĆ 2: KOD HTML DLA STRONY GŁÓWNEJ I AKTUALIZACJI OTA
// ======================================================
String pobierzHTML(String statusMeteo) {
  String html = F("<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>ESP Repeater</title><style>body{font-family:Arial;margin:20px;background:#f4f4f4;text-align:center;}.card{background:white;padding:20px;border-radius:8px;max-width:400px;margin:auto;box-shadow:0 4px 6px rgba(0,0,0,0.1);}input,select,button{width:100%;padding:10px;margin:8px 0;box-sizing:border-box;border:1px solid #ccc;border-radius:4px;}button{background:#007bff;color:white;border:none;cursor:pointer;font-weight:bold;}button:hover{background:#0056b3;}.scan-btn{background:#28a745;}.scan-btn:hover{background:#218838;}.cb-label{display:flex;align-items:center;text-align:left;font-size:14px;margin-bottom:10px;}input[type=checkbox]{width:auto;margin-right:10px;}.ota-btn{background:#6c757d;margin-top:15px;}.ota-btn:hover{background:#5a6268;}</style></head><body><div class='card'><h2>ESP Wi-Fi Repeater</h2><p>");
  html += statusMeteo;
  html += F("</p><hr style='border:0;border-top:1px solid #eee;margin:15px 0;'><form action='/save' method='POST'><label>Wybierz siec:</label><select name='ssid' id='ssid_select'><option value=''>Kliknij Skanuj sieć...</option></select><button type='button' class='scan-btn' onclick='scanNetworks()'>🔍 Skanuj sieć</button><label>Haslo:</label><input type='password' name='pass' id='pass_input' placeholder='Wpisz haslo'><label class='cb-label'><input type='checkbox' onclick='togglePass()'> Pokaz haslo</label><button type='submit'>💾 Zapisz i Uruchom Repeater</button></form><hr style='border:0;border-top:1px solid #eee;margin:15px 0;'><button type='button' class='ota-btn' onclick='window.location.href=\"/update\"'>⚙️ Aktualizacja systemu (OTA)</button></div><script>function togglePass(){var x=document.getElementById('pass_input');x.type=x.type==='password'?'text':'password';}function scanNetworks(){const b=document.querySelector('.scan-btn');const s=document.getElementById('ssid_select');b.innerText='Skanowanie...';b.disabled=true;fetch('/scan').then(r=>r.json()).then(data=>{s.innerHTML='';if(data.length===0){s.innerHTML='<option value=\"\">Nie znaleziono sieci</option>';}else{data.forEach(n=>{let o=document.createElement('option');o.value=n.ssid;o.innerText=n.ssid+' ('+n.rssi+' dBm)';s.appendChild(o);});}b.innerText='🔍 Skanuj sieć';b.disabled=false;});}</script></body></html>");
  return html;
}

const char OTA_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>OTA</title>
<style>body{font-family:Arial;margin:20px;background:#f4f4f4;text-align:center;}.card{background:white;padding:25px;border-radius:8px;max-width:400px;margin:auto;box-shadow:0 4px 6px rgba(0,0,0,0.1);}input,button{width:100%;padding:12px;margin:10px 0;box-sizing:border-box;border:1px solid #ccc;border-radius:4px;}button{background:#dc3545;color:white;border:none;cursor:pointer;font-weight:bold;}button:hover{background:#bd2130;}#prg_container{display:none;margin-top:15px;}#bar{width:0%;height:20px;background:#28a745;text-align:center;color:white;line-height:20px;border-radius:4px;font-size:12px;transition:width 0.2s;}.loader{display:none;margin:15px auto;border:5px solid #f3f3f3;border-top:5px solid #dc3545;border-radius:50%;width:40px;height:40px;animation:spin 1s linear infinite;}@keyframes spin{0%{transform:rotate(0deg);}100%{transform:rotate(360deg);}}</style></head><body>
<div class='card'><h2>Aktualizacja Firmware</h2><div id='form_box'><form method='POST' action='/update' enctype='multipart/form-data' onsubmit='startUpload()'><label style='display:block;text-align:left;margin-bottom:5px;'>Wybierz plik .bin:</label><input type='file' name='update' accept='.bin' required><button type='submit'>🚀 Rozpocznij instalację</button></form></div><div id='prg_container'><h3>Wgrywanie pliku do pamięci...</h3><div style='background:#eee;border-radius:4px;'><div id='bar'>0%</div></div><div class='loader' id='spin_box'></div></div></div>
<script>function startUpload(){document.getElementById('form_box').style.display='none';document.getElementById('prg_container').style.display='block';document.getElementById('spin_box').style.display='block';var p=0;var interval=setInterval(function(){p+=2;if(p<=98){document.getElementById('bar').style.width=p+'%';document.getElementById('bar').innerText=p+'%';}},100);window.addEventListener('unload',function(){clearInterval(interval);});}</script></body></html>
)=====";

const char OTA_SUCCESS[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>Sukces</title>
<style>body{font-family:Arial;margin:50px;background:#f4f4f4;text-align:center;}.card{background:white;padding:30px;border-radius:8px;max-width:400px;margin:auto;box-shadow:0 4px 6px rgba(0,0,0,0.1);}</style>
<script>var s=10;setInterval(function(){s--;document.getElementById('cnt').innerText=s;if(s<=0){window.location.href='/';}},1000);</script></head><body>
<div class='card'><h2 style='color:#28a745;'>Wgrano pomyślnie!</h2><p>Urządzenie właśnie się restartuje.</p><p>Powrót do strony głównej za <span id='cnt' style='font-weight:bold;color:#007bff;'>10</span> sekund...</p></div></body></html>
)=====";

// ======================================================
// CZĘŚĆ 3: LOGIKA SERWERA, INICJALIZACJA I PĘTLA GŁÓWNA
// ======================================================
void handleRoot() {
  String statusMeteo = "Status: ";
  if (WiFi.status() == WL_CONNECTED) {
    statusMeteo += "<b style='color:green;'>Polaczono! (IP: " + WiFi.localIP().toString() + ", Ch: " + String(WiFi.channel()) + ")</b>";
  } else { statusMeteo += "<b style='color:red;'>Rozlaczony (Brak internetu)</b>"; }
  server.send(200, "text/html", pobierzHTML(statusMeteo));
}

void handleScan() {
  int n = WiFi.scanNetworks(false, true); String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
  }
  json += "]"; WiFi.scanDelete(); server.send(200, "application/json", json);
}

void handleSave() {
  String ssid = server.arg("ssid"); String pass = server.arg("pass");
  if (ssid.length() > 0) {
    zapiszSiec(ssid, pass);
    server.send(200, "text/html", "<html><body><h2>Gotowe!</h2><p>ESP restartuje sie...</p></body></html>");
    delay(2000); ESP.restart();
  } else { server.send(400, "text/plain", "Blad danych!"); }
}

void setup() {
  Serial.begin(115200); LittleFS.begin(); WiFi.mode(WIFI_AP_STA);
  IPAddress local_IP(192, 168, 4, 1); IPAddress gateway(192, 168, 4, 1); IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  auto &dhcpServer = WiFi.softAPDhcpServer();
  dhcpServer.setDns(IPAddress(8, 8, 8, 8)); 
  WiFi.softAP(CONFIG_SSID);

  // Bezpieczna podstrona OTA chroniona hasłem (admin / admin123)
  server.on("/update", HTTP_GET, []() {
    if (!server.authenticate("admin", "admin123")) { return server.requestAuthentication(); }
    server.send_P(200, "text/html", OTA_INDEX);
  });
  server.on("/update", HTTP_POST, []() {
    if (!server.authenticate("admin", "admin123")) { return server.send(401, "text/plain", "Brak autoryzacji!"); }
    server.send_P(200, "text/html", OTA_SUCCESS);
    delay(2000); ESP.restart();
  }, []() {
    HTTPUpload& u = server.upload();
    if (u.status == UPLOAD_FILE_START) {
      Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000, U_FLASH);
    } else if (u.status == UPLOAD_FILE_WRITE) {
      Update.write(u.buf, u.currentSize);
    } else if (u.status == UPLOAD_FILE_END) {
      Update.end(true);
    }
  });

  server.on("/", handleRoot); server.on("/scan", handleScan); server.on("/save", handleSave);
  server.begin();
  String zapSSID, zapPASS;
  if (wczytajSiec(zapSSID, zapPASS)) { WiFi.begin(zapSSID.c_str(), zapPASS.c_str()); }
}

void loop() {
  server.handleClient();
  if (millis() - ostatniaKontrola > interwalKontroli) {
    ostatniaKontrola = millis();
    if (WiFi.status() == WL_CONNECTED) {
      czasBezNetu = 0;
      err_t napt_err = ip_napt_init(IP_NAPT_MAX, IP_PORTMAP_MAX);
      if (napt_err == ERR_OK) { ip_napt_enable_no(SOFTAP_IF, 1); }
    } else {
      String zapSSID, zapPASS;
      if (wczytajSiec(zapSSID, zapPASS)) {
        czasBezNetu += interwalKontroli;
        if (czasBezNetu == 30000) {
          int n = WiFi.scanNetworks(false, true);
          for (int i = 0; i < n; ++i) {
            if (WiFi.SSID(i) == zapSSID) { WiFi.begin(zapSSID.c_str(), zapPASS.c_str(), WiFi.channel(i)); break; }
          }
          WiFi.scanDelete();
        }
        if (czasBezNetu >= 180000) { ESP.restart(); } 
      }
    }
  }
}
//koniec
