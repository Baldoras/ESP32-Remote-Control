# ESP32-S3 Remote Control with Multi-Page UI

**Professionelle Fernsteuerung für Kettenfahrzeuge mit ESP-NOW, Touch-Display und SD-Logging**

---

## Kompatibilitätshinweis

**Dieses Projekt ist für die Arduino-IDE ausgelegt und verwendet ESP32 core 3.3.0**

---

## 📋 Projekt-Übersicht

Dieses Projekt ist eine vollständig ausgestattete, batteriegetriebene Fernsteuerung für ein **Kettenfahrzeug**. Die Hardware basiert auf einem **ESP32-S3-N16R8** mit einem **4" TFT-Touchdisplay (ST7796)**, analogem Joystick und umfassendem Logging auf SD-Karte. Die Kommunikation mit dem Fahrzeug erfolgt über **ESP-NOW** (2.4 GHz).

### Hauptmerkmale

- **Multi-Page Touch-UI** mit Header/Footer-System
- **ESP-NOW Kommunikation** mit TLV-Protokoll
- **2S LiPo Batterie-Monitoring** mit Auto-Shutdown
- **Analoger 2-Achsen Joystick** mit Deadzone & Kalibrierung
- **SD-Karte Logging** (Boot, Battery, Connection, Errors)
- **Konfigurierbar** via `config.json` auf SD-Karte

---

## 🔧 Hardware

### Hauptkomponenten

| Komponente | Modell | Beschreibung |
|------------|--------|--------------|
| **Microcontroller** | ESP32-S3-N16R8 | 240MHz Dual-Core, 16MB Flash, 8MB PSRAM |
| **Display** | ST7796 4" TFT | 480x320 Pixel, 65K Farben, SPI |
| **Touch** | XPT2046 | Resistiver Touch-Controller, IRQ-Support |
| **SD-Karte** | MicroSD | Logging & Konfiguration |
| **Joystick** | 2-Achsen analog | 12-Bit ADC (0-4095), Taster |
| **Batterie** | 2S LiPo | 7.4V nominal (6.6V - 8.4V) |
| **Spannungssensor** | 0-25V Modul | Batterie-Monitoring mit ADC |
| **Backlight** | PN2222A NPN | PWM-gesteuert (0-255) |
| **Step-Down buck** | 2 x XL4015 | Speißt eine 5V und eine 3,3V Schiene |

### Pinbelegung

#### Display & Touch (HSPI)
```
TFT_CS    = GPIO10   | TOUCH_CS  = GPIO5
TFT_DC    = GPIO9    | TOUCH_IRQ = GPIO6
TFT_MOSI  = GPIO11   | Shared MOSI/MISO/SCK
TFT_MISO  = GPIO13   | (HSPI Bus)
TFT_SCK   = GPIO12   |
TFT_BL    = GPIO16   | PWM via PN2222A
```

#### SD-Karte (VSPI - separater Bus!)
```
SD_CS   = GPIO38
SD_MOSI = GPIO40
SD_MISO = GPIO41
SD_SCK  = GPIO39
```

#### Joystick & Sensoren
```
JOY_X         = GPIO1  (ADC)
JOY_Y         = GPIO2  (ADC)
JOY_BTN       = GPIO42 (Digital)
VOLTAGE_SENSE = GPIO4  (ADC, 0-25V Modul)
```

---

## 💻 Software-Architektur

### Modulares Design

```
ESP32-Remote-UI.ino          // Hauptprogramm
├── DisplayHandler           // TFT + Backlight + UIManager
├── TouchManager             // XPT2046 mit IRQ & Kalibrierung
├── BatteryMonitor           // Spannungsmessung + Auto-Shutdown
├── JoystickHandler          // 2-Achsen ADC mit Deadzone
├── SDCardHandler            // SD-Karte Mount + File Operations
├── LogHandler               // JSON-Logging (Boot/Battery/Connection/Error)
├── ESPNowManager            // ESP-NOW mit TLV-Protokoll
├── PowerManager             // Deep-Sleep + Wake-up Management
├── UserConfig               // Runtime Config-Management (SD + UI)
├── ConfigManager            // Config laden/speichern (JSON)
├── SerialCommandHandler     // USB Debug-Interface
├── PageManager              // Multi-Page Navigation
└── UI-Widgets               // Button, Label, Slider, ProgressBar, CheckBox, etc.
```

### Konfigurationssystem

- **setupConf.h**: Hardware-Konstanten (GPIO-Pins, Display-Settings, NICHT ÄNDERN)
- **userConf.h**: User-Defaults (Backlight, Touch-Kalibrierung, ESP-NOW)
- **config.json**: Runtime-Config auf SD-Karte (überschreibt userConf.h)
- **UserConfig-Klasse**: Verwaltet Config-Änderungen zur Laufzeit

### UI-System

- **GlobalUI**: Zentrales Header/Footer-Management
  - Header (0-40px): Zurück-Button, Titel, Battery-Icon
  - Footer (300-320px): Status-Text
- **PageManager**: Verwaltet 5 Seiten mit Navigation
- **Widget-Library**: Button, Label, Slider, ProgressBar, CheckBox, RadioButton, TextBox

### Multi-Threading (FreeRTOS)

```
Core 0: WiFi/ESP-NOW
├── RX-Queue: WiFi Callback → Main-Thread
└── ESP-NOW Send/Receive (Hardware)

Core 1: Main Loop
├── Display & UI Updates
├── Touch Handling
├── Joystick Auslesen (20ms Update)
├── Battery Monitoring (1s Update)
└── ESP-NOW Queue Processing
```

**Hinweis:** Worker-Task wurde entfernt, ESP-NOW läuft direkt mit RX-Queue.

---

## 📡 ESP-NOW Kommunikation

### TLV-Protokoll

```
[MAIN_CMD 1B] [TOTAL_LEN 1B] [SUB_CMD 1B] [LEN 1B] [DATA...] ...
```

**Beispiel - Joystick-Daten senden:**
```cpp
EspNowPacket packet;
packet.begin(MainCmd::DATA_REQUEST)
      .addInt16(DataCmd::JOYSTICK_X, joyX)
      .addInt16(DataCmd::JOYSTICK_Y, joyY)
      .addByte(DataCmd::JOYSTICK_BTN, btnState);

espnow.send(peerMac, packet);
```

### Vordefinierte Commands

| MainCmd | Beschreibung |
|---------|--------------|
| `HEARTBEAT` | Keep-Alive alle 500ms |
| `DATA_REQUEST` | Joystick/Sensor-Daten |
| `DATA_RESPONSE` | Telemetrie vom Fahrzeug |

| DataCmd | Typ | Beschreibung |
|---------|-----|--------------|
| `JOYSTICK_X/Y` | int16_t | -100 bis +100 |
| `MOTOR_LEFT/RIGHT` | int16_t | -100 bis +100 |
| `BATTERY_VOLTAGE` | uint16_t | mV |
| `BATTERY_PERCENT` | uint8_t | 0-100% |
| `BUTTON_STATE` | uint8_t | Bitmask |

---

## 💾 SD-Karte Features

### Logging (Linux-style Format)

```
// boot.log
[2024-12-21 14:32:01] [INFO] [BOOT] Boot started: reason=PowerOn, heap=245632 bytes, version=1.0.0
[2024-12-21 14:32:02] [INFO] [BOOT] Init Display: OK
[2024-12-21 14:32:03] [INFO] [BOOT] Init Touch: OK

// battery.log
[2024-12-21 14:33:00] [INFO] [BATTERY] voltage=7.85V, percent=78%, low=false, charging=false
[2024-12-21 14:34:00] [INFO] [BATTERY] voltage=7.82V, percent=77%, low=false, charging=false

// connection.log
[2024-12-21 14:32:05] [INFO] [ESPNOW] Peer connected: AA:BB:CC:DD:EE:FF
[2024-12-21 14:33:05] [INFO] [ESPNOW] Heartbeat sent to AA:BB:CC:DD:EE:FF
[2024-12-21 14:34:05] [WARNING] [ESPNOW] Peer timeout: AA:BB:CC:DD:EE:FF

// error.log
[2024-12-21 14:35:00] [ERROR] [Touch] XPT2046 timeout (code=2)
```

### Konfiguration (`config.json`)

```json
{
  "backlight_default": 128,
  "touch_min_x": 100,
  "touch_max_x": 4000,
  "touch_min_y": 100,
  "touch_max_y": 4000,
  "touch_threshold": 600,
  "espnow_heartbeat": 500,
  "espnow_timeout": 2000,
  "autoshutdown": true,
  "debug_serial": true
}
```

**Vorteil:** Hardware-Defaults in `setupConf.h`, User-Defaults in `userConf.h`.

---

## 🎮 UI-Seiten

### 1. HomePage
- Willkommensbildschirm
- Navigation zu anderen Seiten
- Live-Status: Remote-Battery & ESP-NOW Connection
- 4 Buttons: Remote Control, Connection, Settings, System Info

### 2. RemoteControlPage
- **Joystick-Visualisierung** (2D-Kreis mit Position)
- **X/Y-Werte** live angezeigt (-100 bis +100)
- **Connection-Status** (Connected/Disconnected)
- **Fahrzeug-Battery-Anzeige** (ProgressBar + Spannung)

### 3. ConnectionPage
- **ESP-NOW Pairing/Unpairing**
- **Eigene MAC-Adresse** (Remote)
- **Peer MAC-Adresse** (Fahrzeug)
- **Status**: Disconnected / Paired (Waiting) / Connected
- **Buttons**: PAIR, DISCONNECT

### 4. SettingsPage
- **Backlight-Slider** (PWM 0-255, live Anpassung)
- **Auto-Shutdown-Checkbox** (live aktivieren/deaktivieren)
- **Joystick Center-Kalibrierung** (Button)
- **Hinweis**: Weitere Config via `config.json` auf SD

### 5. InfoPage
- **System-Informationen**:
  - Hardware: ESP32-S3, Flash, PSRAM
  - Display: ST7796 480x320, Touch-Status
  - Remote-Battery: Spannung, Prozent
  - SD-Karte: Freier Speicher
  - ESP-NOW: Init-Status, Connected
  - Joystick: X/Y (raw + mapped), Neutral-Status
  - System: Free Heap, Uptime
- **Refresh-Button** (manuelles Update)

---

## 🔋 Batterie-Management

### Auto-Shutdown

```cpp
// BatteryMonitor.cpp - Unterspannungsschutz
if (currentVoltage <= VOLTAGE_SHUTDOWN) {  // 6.6V = 3.3V/Zelle
    sdCard.logError("Battery", ERR_BATTERY_CRITICAL, "Critical voltage!");
    esp_deep_sleep_start();  // ESP32 ausschalten
}
```

### Warnstufen

| Spannung | Status | Aktion |
|----------|--------|--------|
| 8.4V - 7.0V | ✅ OK | Grün |
| 7.0V - 6.6V | ⚡ LOW | Orange, Warnung alle 10s |
| ≤ 6.6V | ⚠️ CRITICAL | Rot, **Auto-Shutdown** |

---

## 🚀 Installation & Setup

### 1. Arduino IDE Vorbereitung

```bash
# ESP32 Board Package
https://espressif.github.io/arduino-esp32/package_esp32_index.json

# Board Manager: "esp32" by Espressif (v3.0.0+)

# Libraries (via Library Manager):
- TFT_eSPI (v2.5.43+)
- XPT2046_Touchscreen (v1.4+)
- ArduinoJson (v7.x)
```

### 2. TFT_eSPI Konfiguration

**Eigene `User_Setup.h` verwenden:**

`Arduino/libraries/TFT_eSPI/User_Setup_Select.h`:
```cpp
// Alle vordefinierten Setups auskommentieren!
// #include <User_Setups/Setup25_TTGO_T_Display.h>

// Dann die Pins direkt in User_Setup.h eintragen:
```

**ODER:** Pins direkt in `setupConf.h` definieren und via `-D` Flags übergeben.

### 3. Hardware Verkabelung

#### Backlight-Schaltung (NPN+PNP)

**Siehe Schaltplan im Repo:**

```
GPIO16 → 1kΩ → NPN-Basis (2N2222A)
NPN-Emitter → GND
NPN-Kollektor → PNP-Emitter (2N3906)
PNP-Basis → 10kΩ → +3.3V
PNP-Kollektor → TFT_BL+ (Display Backlight)
TFT_BL- → GND
10kΩ Pull-Up (PNP-Basis → 3.3V)
220Ω Strombegrenzung (optional, PNP-Kollektor)
```

**Funktionsweise:**
- GPIO16 HIGH → NPN leitet → PNP-Basis LOW → **Backlight AN**
- GPIO16 LOW → NPN sperrt → PNP-Basis HIGH → **Backlight AUS**
- PWM auf GPIO16 → Helligkeitssteuerung (0-255)

### 4. Code hochladen

**Board-Einstellungen (Arduino IDE):**
```
Board: "ESP32S3 Dev Module"
Flash Size: 16MB (128Mb)
PSRAM: "OPI PSRAM"
Partition Scheme: "16M Flash (3MB APP/9.9MB FATFS)"
Upload Speed: 921600
Core Debug Level: "None" (oder "Info" für Debugging)
```

**Kompilieren & Hochladen:**
```bash
Sketch → Upload
```

### 5. SD-Karte vorbereiten

```bash
# 1. FAT32 formatieren (max. 32GB)
# 2. Optional: config.json erstellen
# 3. In SD-Slot einlegen
# 4. Beim ersten Boot werden Logs automatisch erstellt
```

**Beispiel `config.json`:**
```json
{
  "backlight_default": 200,
  "touch_min_x": 150,
  "touch_max_x": 3900,
  "touch_min_y": 200,
  "touch_max_y": 3850,
  "touch_threshold": 500,
  "espnow_heartbeat": 500,
  "espnow_timeout": 2000,
  "debug_serial": true
}
```

---

## 📊 Debugging & Monitoring

### Serial Monitor (115200 Baud)

```cpp
#define DEBUG_SERIAL true  // setupConf.h

// Output-Beispiel beim Boot:
╔════════════════════════════════════════╗
║   ESP32-S3 Remote Control Startup      ║
║   WITH SD-CARD LOGGING                 ║
╚════════════════════════════════════════╝

→ SD-Card...
  SD-Card OK
→ GPIO...
  GPIO OK
→ Display...
  Display OK
→ Touch...
  Touch OK
→ Battery...
  Battery OK
  Voltage: 7.85V (78%)
→ Joystick...
  Joystick initialisiert
  X-Pin=1, Y-Pin=2
→ ESP-NOW...
  ESP-NOW OK
  MAC: AA:BB:CC:DD:EE:FF

Setup complete!
   Setup-Zeit: 1234 ms
```

### Joystick-Kalibrierung

```bash
# Serial Monitor Output:
Value X: -45 | Raw X: 1234
Value Y: 78  | Raw Y: 3456

# Für Center-Kalibrierung:
# 1. Joystick neutral halten
# 2. Settings → "Calibrate Center" Button drücken
# 3. Neue Center-Werte werden gespeichert
```

### SD-Karte Logs auswerten

```bash
# Linux/Mac:
cat boot.log | grep "ERROR"
cat battery.log | grep "low=true"
cat connection.log | awk '/timeout/ {print}'

# Windows (PowerShell):
Select-String -Path "boot.log" -Pattern "ERROR"
Select-String -Path "battery.log" -Pattern "low=true"
```

---

## 🖥️ SerialCommandHandler (USB Debug-Interface)

Der SerialCommandHandler bietet ein **Command-Line-Interface** über die USB-Verbindung für Debugging und System-Management.

### Verfügbare Befehle

```bash
# Log-Management
logs                    # Listet alle Log-Dateien mit Größe auf
read <file>            # Liest komplette Log-Datei
tail <file> <n>        # Zeigt letzte N Zeilen (z.B. tail boot.log 20)
head <file> <n>        # Zeigt erste N Zeilen
clear <file>           # Löscht eine Log-Datei
clearall               # Löscht ALLE Log-Dateien (Vorsicht!)

# System-Informationen
sysinfo                # Zeigt Hardware-/System-Info (Chip, Flash, PSRAM, Heap)
battery                # Zeigt Battery-Status (Spannung, Prozent, Status)
espnow                 # Zeigt ESP-NOW Status (Peers, Connection)

# Konfiguration
config                 # Zeigt komplette Konfiguration
config list            # Listet alle Config-Keys auf
config get <key>       # Zeigt einen Config-Wert
config set <key> <val> # Setzt einen Config-Wert (Runtime)
config save            # Speichert Config auf SD (config.json)
config reset           # Lädt Standard-Config (userConf.h)

# Hilfe
help                   # Zeigt alle verfügbaren Befehle
```

### Beispiel-Session

```
> help
Available Commands:
  logs, read, tail, head, clear, clearall
  sysinfo, battery, espnow
  config, config list, config get, config set, config save
  
> logs
Log Files:
  /logs/boot.log        [2.3 KB]
  /logs/battery.log     [15.7 KB]
  /logs/connection.log  [8.1 KB]
  /logs/error.log       [0.5 KB]

> tail boot.log 5
[2024-12-21 14:32:05] [INFO] [BOOT] Init ESP-NOW: OK
[2024-12-21 14:32:05] [INFO] [BOOT] Boot complete: 2345ms
[2024-12-21 14:32:05] [INFO] [BOOT] Free heap: 245632 bytes

> battery
Battery Status:
  Voltage: 7.85V
  Percent: 78%
  Status: OK
  Low: false
  Charging: false

> config get backlight_default
backlight_default = 128

> config set backlight_default 200
Config updated: backlight_default = 200
Use 'config save' to persist to SD card
```

### Verwendung

Der SerialCommandHandler wird automatisch in `setup()` initialisiert und läuft im Hintergrund:

```cpp
// In ESP32-Remote-UI.ino
SerialCommandHandler serialCmd;

void setup() {
    Serial.begin(115200);
    // ... andere Initialisierung
    
    serialCmd.begin(&sdCard, &logger, &battery, &espNow, &userConfig);
}

void loop() {
    serialCmd.update();  // Prüft auf eingehende Befehle
    // ... Rest der Loop
}
```

---

## 🎯 Verwendung

### Erstinbetriebnahme

1. **Remote einschalten** → HomePage erscheint
2. **Connection** → Eigene MAC notieren
3. **Fahrzeug einschalten** (separater ESP32)
4. **Connection** → Peer MAC eingeben (Fahrzeug)
5. **PAIR** Button drücken
6. **Warten** auf "Connected" Status
7. **Remote Control** → Joystick steuert Fahrzeug

### Troubleshooting

| Problem | Lösung |
|---------|--------|
| **Display bleibt schwarz** | Backlight-Schaltung prüfen (NPN+PNP), GPIO16 Check |
| **Touch reagiert nicht** | TOUCH_CS auf HIGH? Kalibrierung in config.json |
| **Joystick driftet** | Center-Kalibrierung durchführen, Deadzone erhöhen |
| **ESP-NOW nicht verbunden** | MAC-Adresse korrekt? Beide Geräte auf Kanal 0? |
| **SD-Karte nicht erkannt** | FAT32? CS-Pin korrekt? SPI-Frequenz reduzieren |
| **Battery shutdown** | LiPo aufladen! Spannung < 6.6V = Auto-Shutdown |

---

## 📐 Technische Details

### Memory Management

```cpp
// ESP32-S3 Memory Layout:
Flash:  16MB (Code + SPIFFS)
PSRAM:  8MB  (UI-Widgets, Buffers)
SRAM:   512KB (Stack, Heap)

// Optimierungen:
- UILayout: 1x Header/Footer (nicht pro Page!)
- UI-Widgets: Lazy Creation (nur bei build())
- ESP-NOW: Queue-basiert (RX/TX/Result)
- JSON: ArduinoJson V7 (optimierte Deserialisierung)
```

### Power Consumption

```cpp
// Geschätzte Stromaufnahme (Remote):
Display Backlight (max): ~200mA @ 3.3V
ESP32-S3 Active:         ~100mA @ 5V
Display + Touch:         ~50mA @ 5V
ESP-NOW TX (continuous): ~120mA @ 3.3V (nicht getestet)
-------------------------------------------
TOTAL (worst case):      ~470mA @ 3.3V (berechnet)

// Getestet und gemessen
TOTAL ohne ESP-NOW       ~130mA @ 8,4V (Akku-Ausgang)


// 2S LiPo 3000mAh:
Laufzeit: ~6 - 8 Stunden (kontinuierlich und getestet)
```

### Joystick Deadzone

```cpp
// JoystickHandler.cpp - applyDeadzone()
// Standard: 10% Deadzone

Raw ADC:    0 ───── 2048 ───── 4095
Mapped:   -100 ───── 0 ───── +100
Deadzone:       [-10 ... +10] → 0

// Verhindert Drift durch ADC-Rauschen
// Konfigurierbar via setDeadzone(0-100)
```

---

## 🔧 Anpassungen & Tuning

### Joystick-Empfindlichkeit

```cpp
// JoystickHandler.cpp - setUpdateInterval()
joystick.setUpdateInterval(20);  // 20ms = 50Hz (Standard)
joystick.setUpdateInterval(50);  // 50ms = 20Hz (langsamer)
```

### ESP-NOW Heartbeat

```cpp
// userConf.h oder config.json
#define ESPNOW_HEARTBEAT_INTERVAL 500  // 500ms = 2Hz
#define ESPNOW_TIMEOUT_MS 2000         // 2s Timeout
```

### Display Helligkeit

```cpp
// Settings-Page: Slider 0-255
// Oder in userConf.h oder config.json:
#define BACKLIGHT_DEFAULT 128  // 50% Helligkeit
```

---

## 📜 Lizenz

MIT License - Siehe [LICENSE](LICENSE)

---

## 🙏 Credits

- **TFT_eSPI** by Bodmer
- **XPT2046_Touchscreen** by Paul Stoffregen
- **ArduinoJson** by Benoit Blanchon
- **ESP-NOW** Framework by Espressif

---

## 📧 Kontakt

**Entwickelt für ein Kettenfahrzeug-Projekt**

Fragen oder Verbesserungsvorschläge? Erstelle ein Issue auf GitHub!

---

## 📸 Hardware-Bilder (in arbeit)

- **ESP32-S3-N16R8 Board Pinout** - Vollständige Pin-Belegung
- **ST7796 4" TFT Display mit SD-Slot** - Display-Modul
- **Backlight-Schaltung (NPN+PNP)** - Schaltplan für normale Logik
- **Kettenfahrzeug-Chassis** - Beispiel-Hardware