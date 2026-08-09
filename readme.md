# ESP8266 Wi-Fi NAT Repeater (Arduino Version)
📄 Licencja

Projekt jest udostępniany w stanie "takim, jaki jest" na licencji
GNU General Public License v3.0 (GPLv3). 

Używasz go na własną odpowiedzialność.
Starano się bardzo, aby program ten nic nie popsuł.

Autonomiczny, bezobsługowy wzmacniacz sieci Wi-Fi (2.4 GHz) zbudowany na bazie mikrokontrolera **ESP-01S (ESP8266)** w środowisku Arduino IDE. Urządzenie działa jako niezależny router z translacją adresów NAT, posiada wbudowany system pamięci nieulotnej oraz inteligentną architekturę samonaprawy połączenia.

## 🚀 Kluczowe Funkcje

*   **Sprzętowy Routing NAT/NAPT:** Układ tworzy niezależną podsieć (`192.168.4.X`), izolując podłączone urządzenia od sieci głównej. Domowy router widzi repeater jako jednego klienta.
*   **Auto-Channel Hopping (Automatyczny Przeskok):** Urządzenie stale monitoruje stan połączenia. Jeśli router główny zmieni kanał nadawania, ESP po 30 sekundach automatycznie przeskanuje eter, namierzy nową częstotliwość i przełączy swoje radio.
*   **Trwały Zapis Konfiguracji (LittleFS):** Nazwa sieci (SSID) oraz hasło są trwale zapisywane w pamięci Flash. Po zaniku zasilania wzmacniacz automatycznie wznawia pracę bez potrzeby ponownej konfiguracji.
*   **Wbudowany Serwer DHCP z Wtryskiem DNS:** Serwer DHCP automatycznie konfiguruje klientów sieci `MyAP_Arduino`, wymuszając stabilny serwer DNS Google (`8.8.8.8`). Rozwiązuje to problem błędów typu *Nie odnaleziono serwera*.
*   **Nowoczesny Panel Aktualizacji OTA przez WWW:** Bezprzewodowa aktualizacja oprogramowania (`/update`) zabezpieczona protokołem *HTTP Basic Auth*. Panel zawiera dynamiczny pasek postępu (0-100%), animację ładowania oraz licznik restartu.
*   **Zintegrowany Watchdog:** W przypadku krytycznej, długotrwałej utraty sygnału (powyżej 3 minut), sprzętowy licznik wywołuje twardy reset układu (`ESP.restart()`) w celu samonaprawy.

## 🛠️ Specyfikacja Sprzętowa i Rozpiska Pinów

Moduł ESP-01S zasilany jest wyłącznie napięciem **3.3V**. Podłączenie 5V bez regulatora trwale uszkodzi układ.

### Schemat Wyprowadzeń (Widok od góry, antena po lewej):
```text
  +-----------------------+

  |  [ ANTENA PCB ]       |
  |                       |
  |  (GND)  [ ]   [ ] (TX)|
  | (GPIO2) [ ]   [ ] (RX)|
  | (GPIO0) [ ]   [ ] (EN)|
  |  (3.3V) [ ]   [ ] (RST|
  +-----------------------+
```

### Opis Pinów dla Trybu Pracy i Programowania:

| Nazwa Pinu | Rola w układzie | Połączenie (Tryb Pracy) | Połączenie (Programowanie) |
| :--- | :--- | :--- | :--- |
| **VCC** | Zasilanie główne | Zasilacz 3.3V | 3.3V z programatora CH340 |
| **GND** | Masa układu | Masa (GND) zasilacza | GND z programatora CH340 |
| **EN / CH_PD**| Aktywacja układu | **Wymagane** zwarcie do 3.3V | **Wymagane** zwarcie do 3.3V |
| **GPIO0** | Tryb rozruchu | Pozostawić wolny (High) | **Wymagane** zwarcie do # ESP8266 Wi-Fi NAT Repeater (Arduino Version)

Autonomiczny, bezobsługowy wzmacniacz sieci Wi-Fi (2.4 GHz) zbudowany na bazie mikrokontrolera **ESP-01S (ESP8266)** w środowisku Arduino IDE. Urządzenie działa jako niezależny router z translacją adresów NAT, posiada wbudowany system pamięci nieulotnej oraz inteligentną architekturę samonaprawy połączenia.

## 🚀 Kluczowe Funkcje

*   **Sprzętowy Routing NAT/NAPT:** Układ tworzy niezależną podsieć (`192.168.4.X`), izolując podłączone urządzenia od sieci głównej. Domowy router widzi repeater jako jednego klienta.
*   **Auto-Channel Hopping (Automatyczny Przeskok):** Urządzenie stale monitoruje stan połączenia. Jeśli router główny zmieni kanał nadawania, ESP po 30 sekundach automatycznie przeskanuje eter, namierzy nową częstotliwość i przełączy swoje radio.
*   **Trwały Zapis Konfiguracji (LittleFS):** Nazwa sieci (SSID) oraz hasło są trwale zapisywane w pamięci Flash. Po zaniku zasilania wzmacniacz automatycznie wznawia pracę bez potrzeby ponownej konfiguracji.
*   **Wbudowany Serwer DHCP z Wtryskiem DNS:** Serwer DHCP automatycznie konfiguruje klientów sieci `MyAP_Arduino`, wymuszając stabilny serwer DNS Google (`8.8.8.8`). Rozwiązuje to problem błędów typu *Nie odnaleziono serwera*.
*   **Nowoczesny Panel Aktualizacji OTA przez WWW:** Bezprzewodowa aktualizacja oprogramowania (`/update`) zabezpieczona protokołem *HTTP Basic Auth*. Panel zawiera dynamiczny pasek postępu (0-100%), animację ładowania oraz licznik restartu.
*   **Zintegrowany Watchdog:** W przypadku krytycznej, długotrwałej utraty sygnału (powyżej 3 minut), sprzętowy licznik wywołuje twardy reset układu (`ESP.restart()`) w celu samonaprawy.

## 🛠️ Specyfikacja Sprzętowa i Rozpiska Pinów

Moduł ESP-01S zasilany jest wyłącznie napięciem **3.3V**. Podłączenie 5V bez regulatora trwale uszkodzi układ.

### Schemat Wyprowadzeń (Widok od góry, antena po lewej):
```text
  +-----------------------+

  |  [ ANTENA PCB ]       |
  |                       |
  |  (GND)  [ ]   [ ] (TX)|
  | (GPIO2) [ ]   [ ] (RX)|
  | (GPIO0) [ ]   [ ] (EN)|
  |  (3.3V) [ ]   [ ] (RST|
  +-----------------------+
```

### Opis Pinów dla Trybu Pracy i Programowania:

| Nazwa Pinu | Rola w układzie | Połączenie (Tryb Pracy) | Połączenie (Programowanie) |
| :--- | :--- | :--- | :--- |
| **VCC** | Zasilanie główne | Zasilacz 3.3V | 3.3V z programatora CH340 |
| **GND** | Masa układu | Masa (GND) zasilacza | GND z programatora CH340 |
| **EN / CH_PD**| Aktywacja układu | **Wymagane** zwarcie do 3.3V | **Wymagane** zwarcie do 3.3V |
| **GPIO0** | Tryb rozruchu | Pozostawić wolny (High) | **Wymagane** zwarcie do masy (GND) |
| **GPIO2** | Dioda wbudowana | Pozostawić wolny | Pozostawić wolny |
| **RX / TX** | Linie szeregowe | Pozostawić wolne | Podłączyć krzyżowo do TX/RX CH340 |
| **RST** | Reset sprzętowy | Pozostawić wolny | Pozostawić wolny |

---

## 💾 Instrukcja Wymiany Pamięci Flash (Modyfikacja 4MB)
Aby mieć możliwość dalszych modyfikacji i bezproblemowe OTA, wymień pamięc na większą, np 4Mb.
W wersji standartowej OTA działa na "styk" i może ale nie musi powodować niestabilności.
Fabryczna kość pamięci 1MB (SOIC-8) ogranicza przestrzeń na aktualizacje OTA i pliki systemowe. Można ją zastąpić układem wylutowanym ze starej płyty głównej PC (np. kość BIOS serii **Winbond 25Q32 / 25Q64** pracująca na 3.3V).

### Procedura sprzętowa:
1.  Za pomocą stacji Hot-Air lub techniki "dużej kropli cyny" wylutuj oryginalną pamięć z ESP-01S.
2.  Oczyść pady na płytce plecionką rozlutowniczą.
3.  Wylutuj kość BIOS z płyty głównej (upewnij się, że to wersja 3.3V w obudowie SOIC-8). Jeśli obudowa jest nieco szersza (Wide 208mil), delikatnie podegnij nóżki układu do środka.
4.  Wlutuj nową kość na ESP-01S zgodnie z kropką oznaczającą pin nr 1.

### Pierwsze wgranie oprogramowania po wymianie kości:
1.  W Arduino IDE wejdź w **Narzędzia (Tools) ➡️ Flash Size** i zmień wartość z `1MB` na **`4MB (FS:2MB OTA:~1019KB)`**.
2.  Zewrzyj pin **GPIO0 do GND**.
3.  Podłącz programator CH340 pod USB i kliknij **Wgraj**. Proces potrwa chwilę dłużej, ponieważ `esptool` całkowicie wymaże stary BIOS, nadpisze całą pamięć nowym kodem i sformatuje system plików LittleFS (2MB).

---

## ⚙️ Wymagane Ustawienia Kompilacji (Arduino IDE)

*   **Płytka (Board):** `Generic ESP8266 Module`
- Patrz załączony zrzut ekranu (Ustawienia.png) - wersja 1Mb pamięci - przed wymianą.

## 📡 Pierwsze Uruchomienie i Konfiguracja

1.  Po pomyślnym zflashowaniu odłącz pin **GPIO0 od masy (GND)**.
2.  Zasil moduł i odczekaj około 10 sekund.
3.  Wyszukaj na komputerze lub telefonie otwartą sieć Wi-Fi o nazwie: **`MyAP_Arduino`**.
4.  Otwórz przeglądarkę i wejdź na adres: **`192.168.4.1`**.
5.  Kliknij przycisk **🔍 Skanuj sieć**, wybierz swój domowy router z listy, wpisz hasło i kliknij **💾 Zapisz i Uruchom Repeater**.

## 🔐 Bezprzewodowa Aktualizacja Oprogramowania (OTA)

Od momentu pierwszego wgrania kablowego na nową pamięć, programator CH340 nie jest już potrzebny. Aby zaktualizować kod:

1.  W Arduino IDE wybierz menu **Szkic ➡️ Eksportuj skompilowany program** (wygeneruje to plik `.bin` w folderze projektu).
2.  Połącz się z siecią `MyAP_Arduino` i wejdź na stronę: **`192.168.4.1/update`**.
3.  Wpisz dane logowania:
    *   **Użytkownik:** `admin`
    *   **Hasło:** `admin123`
4.  Wskaż wygenerowany plik `.bin` i kliknij **🚀 Rozpocznij instalację**.
5.  Pasek postępu wyświetli stan wgrywania. Po zakończeniu odliczanie (10s) poinformuje o restarcie urządzenia.

## 📊 Ograniczenia Techniczne Układu (ESP-01S)

*   **Przepustowość sieci:** maksymalnie ok. 4–5 Mb/s (ograniczenie procesora 160 MHz przy translacji pakietów NAT).
*   **Pasmo radiowe:** wyłącznie 2.4 GHz (brak obsługi pasma 5 GHz oraz standardów Wi-Fi 6/7).
*   **Maksymalna liczba klientów:** domyślnie zablokowana na maksymalnie 4 urządzenia jednocześnie w celu utrzymania stabilności pamięci RAM (80 KB).
masy (GND) |
| **GPIO2** | Dioda wbudowana | Pozostawić wolny | Pozostawić wolny |
| **RX / TX** | Linie szeregowe | Pozostawić wolne | Podłączyć krzyżowo do TX/RX CH340 |
| **RST** | Reset sprzętowy | Pozostawić wolny | Pozostawić wolny |

---
