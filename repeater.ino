#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <flash_hal.h>
#include <FS.h>
#include <LittleFS.h>
#include <lwip/napt.h>

// 1. Ustawienia stałe sieci i autoryzacji OTA
const char* CONFIG_SSID = "MyAP_Arduino";
const char* OTA_USER = "admin";
const char* OTA_PASS = "admin123"; 

ESP8266WebServer server(80);

unsigned long ostatniaKontrola = 0;
const unsigned long interwalKontroli = 10000; 
unsigned long czasBezNetu = 0;

// 2. STYLIZOWANE STRONY HTML (Zdefiniowane na samej górze dla kompilatora)
String pobierzHTML(String statusMeteo) {
  String html = F("<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>ESP Repeater</title><style>body{font-family:Arial;margin:20px;background:#f4f4f4;text-align:center;}.card{background:white;padding:20px;border-radius:8px;max-width:400px;margin:auto;box-shadow:0 4px 6px rgba(0,0,0,0.1);}input,select,button{width:100%;padding:10px;margin:8px 0;box-sizing:border-box;border:1px solid #ccc;border-radius:4px;}button{background:#007bff;color:white;border:none;cursor:pointer;font-weight:bold;}button:hover{background:#0056b3;}.scan-btn{background:#28a745;}.scan-btn:hover{background:#218838;}.cb-label{display:flex;align-items:center;text-align:left;font-size:14px;margin-bottom:10px;}input[type=checkbox]{width:auto;margin-right:10px;}.ota-btn{background:#6c757d;margin-top:15px;}.ota-btn:hover{background:#5a6268;}</style></head><body><div class='card'><h2>ESP Wi-Fi Repeater</h2><p>");
  html += statusMeteo;
  html += F("</p><hr style='border:0;border-top:1px solid #eee;margin:15px 0;'><form action='/save' method='POST'><label>Wybierz siec:</label><select name='ssid' id='ssid_select'><option value=''>Kliknij Skanuj sieć...</option></select><button type='button' class='scan-btn' onclick='scanNetworks()'>🔍 Skanuj sieć</button><label>Haslo:</label><input type='password' name='pass' id='pass_input' placeholder='Wpisz haslo'><label class='cb-label'><input type='checkbox' onclick='togglePass()'> Pokaz haslo</label><button type='submit'>💾 Zapisz i Uruchom Repeater</button></form><hr style='border:0;border-top:1px solid #eee;margin:15px 0;'><button type='button' class='ota-btn' onclick='window.location.href=\"/update\"'>⚙️ Aktualizacja systemu (OTA)</button></div><script>function togglePass(){var x=document.getElementById('pass_input');x.type=x.type==='password'?'text':'password';}function scanNetworks(){const b=document.querySelector('.scan-btn');const s=document.getElementById('ssid_select');b.innerText='Skanowanie...';b.disabled=true;fetch('/scan').then(r=>r.json()).then(data=>{s.innerHTML='';if(data.length===0){s.innerHTML='<option value=\"\">Nie znaleziono sieci</option>';}else{data.forEach(n=>{let o=document.createElement('option');o.value=n.ssid;o.innerText=n.ssid+' ('+n.rssi+' dBm)';s.appendChild(o);});}b.innerText='🔍 Skanuj sieć';b.disabled=false;});}</script></body></html>");
  return html;
}

const char OTA_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>Aktualizacja</title>
<style>body{font-family:Arial;margin:20px;background:#f4f4f4;text-align:center;}.card{background:white;padding:25px;border-radius:8px;max-width:400px;margin:auto;box-shadow:0 4px 6px rgba(0,0,0,0.1);}input,button{width:100%;padding:12px;margin:10px 0;box-sizing:border-box;border:1px solid #ccc;border-radius:4px;}button{background:#dc3545;color:white;border:none;cursor:pointer;font-weight:bold;}button:hover{background:#bd2130;}#prg_container{display:none;margin-top:15px;}#bar{width:0%;height:20px;background:#28a745;text-align:center;color:white;line-height:20px;border-radius:4px;font-size:12px;transition:width 0.2s;}.loader{display:none;margin:15px auto;border:5px solid #f3f3f3;border-top:5px solid #dc3545;border-radius:50%;width:40px;height:40px;animation:spin 1s linear infinite;}@keyframes spin{0%{transform:rotate(0deg);}100%{transform:rotate(360deg);}}</style></head><body>
<div class='card'><h2>Aktualizacja Firmware</h2><div id='form_box'><form method='POST' action='/update' enctype='multipart/form-data' onsubmit='startUpload()'><label style='display:block;text-align:left;margin-bottom:5px;'>Wybierz plik .bin:</label><input type='file' name='update' accept='.bin' required><button type='submit'>🚀 Rozpocznij instalację</button></form></div><div id='prg_container'><h3>Wgrywanie pliku do pamięci...</h3><div style='background:#eee;border-radius:4px;'><div id='bar'>0%</div></div><div class='loader' id='spin_box'></div></div></div>
<script>function startUpload(){document.getElementById('form_box').style.display='none';document.getElementById('prg_container').style.display='block';document.getElementById('spin_box').style.display='block';var p=0;var interval=setInterval(function(){p+=2;if(p<=98){document.getElementById('bar').style.width=p+'%';document.getElementById('bar').innerText=p+'%';}},100);window.addEventListener('unload',function(){clearInterval(interval);});}</script></body></html>
)=====";

const char OTA_SUCCESS[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Sukces</title>
<style>
  body{font-family:Arial;margin:50px;background:#f4f4f4;text-align:center;}
  .card{background:white;padding:30px;border-radius:8px;max-width:550px;margin:auto;box-shadow:0 4px 6px rgba(0,0,0,0.1);}
</style>
</head><body>
<div class="card">
  <h2 style="color: rgb(40, 167, 69);">Wgrano pomyślnie!</h2>
  <p>Urządzenie wykonuje właśnie restart w tle.</p>
  <p style="color: rgb(220, 53, 69); font-weight: bold;">Instrukcja szybkiego powrotu:</p>
  <p style="text-align: left; font-size: 14px; color: rgb(85, 85, 85);">1. Wejdź <b>od razu</b> w menu Wi-Fi.<br>2. Rozłącz ręcznie sieć <b>MyAP_Arduino</b><br>&nbsp;&nbsp;&nbsp; - (wymusi to natychmiastowe przelogowanie bez czekania).<br>3. Połącz się ponownie z <b>MyAP_Arduino</b><br>4. Spawdź czy jestes połaczony z <b>MyAP_Arduino</b> &nbsp; (http://192.168.4.1)  <br>5. Kliknij poniższy przycisk:<br><br>
 </p>
<button id="action_btn" class="btn"
  style="background-color:#ff6a00; color:#fff; border:1px solid #ff6a00;"
  onmouseenter="this.style.backgroundColor='#3245f0'; this.style.borderColor='#3245f0';"
  onmouseleave="this.style.backgroundColor='#ff6a00'; this.style.borderColor='#ff6a00';"
  onclick="window.location.href = window.location.origin + '/'">
  🔄 Powrót do panelu sterowania
</button>

<button id="action_btn" class="btn"
  style="background-color:#ff6a00; color:#fff; border:1px solid #ff6a00;"
  onmouseenter="this.style.backgroundColor='#5dc3e2'; this.style.borderColor='#5dc3e2';"
  onmouseleave="this.style.backgroundColor='#ff6a00'; this.style.borderColor='#ff6a00';"
  onclick="window.location.href='https://duckduckgo.com/'">
  🔄 Wyszukiwarka DuckDuck
</button>

<button id="action_btn" class="btn"
  style="background-color:#ff6a00; color:#fff; border:1px solid #ff6a00;"
  onmouseenter="this.style.backgroundColor='#5dc3e2'; this.style.borderColor='#5dc3e2';"
  onmouseleave="this.style.backgroundColor='#ff6a00'; this.style.borderColor='#ff6a00';"
  onclick="window.location.href='https://google.pl/'">
  🔄 Wyszukiwarka Google
</button>
</div>
</body></html>
)=====";



// 3. Obsługa pamięci Flash (LittleFS)
void zapiszSiec(String s, String p) {
  File f = LittleFS.open("/net.txt", "w");
  if (f) {
    f.println(s);
    f.println(p);
    f.close();
  }
}

bool wczytajSiec(String &s, String &p) {
  if (!LittleFS.exists("/net.txt")) return false;
  File f = LittleFS.open("/net.txt", "r");
  if (!f) return false;
  s = f.readStringUntil('\n');
  p = f.readStringUntil('\n');
  s.trim();
  p.trim();
  f.close();
  return (s.length() > 0);
}

// 4. Obsługa standardowych żądań HTTP
void handleRoot() {
  String statusMeteo = "Status: ";
  if (WiFi.status() == WL_CONNECTED) {
    statusMeteo += "<b style='color:green;'>Połączono z routerem! (IP: " + WiFi.localIP().toString() + ")</b>";
  } else {
    statusMeteo += "<b style='color:red;'>Rozłączony (Brak internetu)</b>";
  }
  server.send(200, "text/html", pobierzHTML(statusMeteo));
}

void handleScan() {
  int n = WiFi.scanNetworks(false, true); 
  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
  }
  json += "]";
  WiFi.scanDelete();
  server.send(200, "application/json", json);
}

void handleSave() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  if (ssid.length() > 0) {
    zapiszSiec(ssid, pass);
    String htmlResponse = "<html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><style>body{font-family:Arial;text-align:center;margin-top:50px;background:#f4f4f4;}.card{background:white;padding:30px;border-radius:8px;max-width:400px;margin:auto;box-shadow:0 4px 6px rgba(0,0,0,0.1);}</style></head><body><div class='card'><h2 style='color:green;'>Gotowe!</h2><p>Dane zostały trwale zapisane. Resetowanie...</p></div></body></html>";
    server.send(200, "text/html", htmlResponse);
    delay(2000);
    ESP.restart();
  }
}

// 5. Obsługa modułu OTA z filtrami autoryzacji
void handleOtaGet() {
  if (!server.authenticate(OTA_USER, OTA_PASS)) {
    return server.requestAuthentication();
  }
  server.send_P(200, "text/html", OTA_INDEX);
}

void handleOtaPost() {
  if (!server.authenticate(OTA_USER, OTA_PASS)) {
    return server.requestAuthentication();
  }
  
  // Wysyłamy stronę sukcesu do przeglądarki
  server.send_P(200, "text/html", OTA_SUCCESS);
  delay(1000);
  
  // POPRAWKA: Oficjalne zamknięcie sieci AP, aby Linux od razu zauważył rozłączenie
  WiFi.softAPdisconnect(true); 
  delay(1000);
  
  // Dopiero teraz bezpieczny restart układu
  ESP.restart();
}


void handleOtaUpload() {
  if (!server.authenticate(OTA_USER, OTA_PASS)) {
    return;
  }
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.setDebugOutput(true);
    WiFiUDP::stopAll();
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace)) { 
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { 
      Serial.printf("Sukces aktualizacji: %u bajtow\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  }
}

// 6. Konfiguracja główna (Setup) i pętla (Loop)
void setup() {
  Serial.begin(115200);
  LittleFS.begin(); 
  
  WiFi.mode(WIFI_AP_STA);
  
  // Wymuszenie szybkiego połączenia z routerem na starcie (Szybki rozruch poniżej 5 sekund)
  String zapisaneSSID, zapisanePASS;
  if (wczytajSiec(zapisaneSSID, zapisanePASS)) {
    Serial.print("Szybkie laczenie z: ");
    Serial.println(zapisaneSSID);
    WiFi.begin(zapisaneSSID.c_str(), zapisanePASS.c_str());
    
    int proby = 0;
    while (WiFi.status() != WL_CONNECTED && proby < 10) {
      delay(500);
      Serial.print(".");
      proby++;
    }
    Serial.println();
  }

  IPAddress local_IP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  
  // Konfiguracja DHCP DNS pod Core 3.1.2
  auto &dhcpServer = WiFi.softAPDhcpServer();
  dhcpServer.setDns(IPAddress(8, 8, 8, 8));
  
  WiFi.softAP(CONFIG_SSID);

  if (WiFi.status() == WL_CONNECTED) {
    ip_napt_init(IP_NAPT_MAX, IP_PORTMAP_MAX);
    ip_napt_enable_no(SOFTAP_IF, 1);
    Serial.println("NAT uruchomiony natychmiast!");
  }

  // Rejestracja ścieżek  panelu OTA
  server.on("/update", HTTP_GET, handleOtaGet);
  server.on("/update", HTTP_POST, handleOtaPost, handleOtaUpload);

  server.on("/", handleRoot);
  server.on("/scan", handleScan);
  server.on("/save", handleSave);
  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - ostatniaKontrola > interwalKontroli) {
    ostatniaKontrola = millis();

    if (WiFi.status() == WL_CONNECTED) {
      czasBezNetu = 0;
      err_t napt_err = ip_napt_init(IP_NAPT_MAX, IP_PORTMAP_MAX);
      if (napt_err == ERR_OK) {
        ip_napt_enable_no(SOFTAP_IF, 1);
      }
    } else {
      if (WiFi.SSID().length() > 0) {
        czasBezNetu += interwalKontroli;
        if (czasBezNetu >= 180000) {
          ESP.restart(); 
        }
      }
    }
  }
}
//koniec