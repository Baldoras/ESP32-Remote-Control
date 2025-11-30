/**
 * ConnectionPage.h
 * 
 * Verbindungs-Seite für ESP-NOW Pairing und Status
 * 
 * Features:
 * - MAC-Adresse Anzeige (eigene + Peer)
 * - Pairing-Button
 * - Verbindungsstatus mit Icon
 * - Signalstärke (RSSI) Balken
 * - Heartbeat-Indikator (LED-Animation)
 * - Letztes Paket Timestamp
 */

#ifndef CONNECTION_PAGE_H
#define CONNECTION_PAGE_H

#include "UIPage.h"
#include "UILabel.h"
#include "UIButton.h"
#include "UIProgressBar.h"
#include "ESPNowManager.h"

class ConnectionPage : public UIPage {
public:
    /**
     * Konstruktor
     * @param ui UIManager Pointer
     * @param tft TFT Display Pointer
     */
    ConnectionPage(UIManager* ui, TFT_eSPI* tft);
    
    /**
     * Destruktor
     */
    ~ConnectionPage();

    /**
     * Seite erstellen (von UIPage geerbt)
     */
    void build() override;

    /**
     * Update (in loop() aufrufen)
     */
    void update() override;

    /**
     * Peer-MAC setzen (für Verbindung)
     * @param mac MAC-Adresse als String (AA:BB:CC:DD:EE:FF)
     */
    void setPeerMac(const char* macStr);

    /**
     * Peer-MAC abrufen
     */
    const char* getPeerMac() const { return peerMacStr; }

    /**
     * Verbindungsstatus aktualisieren
     */
    void updateConnectionStatus();

    /**
     * Signalstärke aktualisieren
     * @param rssi RSSI in dBm (-100 bis 0)
     */
    void updateRSSI(int8_t rssi);

    /**
     * Heartbeat-Indikator animieren
     */
    void updateHeartbeat(bool received);

private:
    // ESP-NOW Manager Referenz
    EspNowManager& espnow;
    
    // Peer-Daten
    char peerMacStr[18];        // "AA:BB:CC:DD:EE:FF\0"
    uint8_t peerMac[6];         // Binäre MAC
    bool isPaired;              // Ist gepaart?
    bool isConnected;           // Ist verbunden?
    
    // RSSI-Daten
    int8_t currentRSSI;         // Aktueller RSSI-Wert
    unsigned long lastPacketTime; // Letztes empfangenes Paket
    
    // Heartbeat-Animation
    bool heartbeatState;        // LED-Status
    unsigned long lastHeartbeat; // Letzter Heartbeat
    
    // UI-Elemente
    UILabel* labelTitle;
    
    // Status-Bereich
    UILabel* labelStatus;
    UILabel* labelStatusValue;
    
    // Eigene MAC
    UILabel* labelOwnMac;
    UILabel* labelOwnMacValue;
    
    // Peer MAC
    UILabel* labelPeerMac;
    UILabel* labelPeerMacValue;
    
    // RSSI-Anzeige
    UILabel* labelRSSI;
    UIProgressBar* barRSSI;
    UILabel* labelRSSIValue;
    
    // Heartbeat-Indikator
    UILabel* labelHeartbeat;
    UILabel* labelHeartbeatLED;
    
    // Letztes Paket
    UILabel* labelLastPacket;
    UILabel* labelLastPacketValue;
    
    // Buttons
    UIButton* btnPair;
    UIButton* btnDisconnect;
    UIButton* btnRefresh;
    
    // Interne Methoden
    void initUI();
    void updateUI();
    void updateRSSIBar();
    void updateHeartbeatLED();
    void updateLastPacketTime();
    
    // Button-Callbacks
    void onPairClicked();
    void onDisconnectClicked();
    void onRefreshClicked();
    
    // RSSI zu Prozent konvertieren (-100 dBm = 0%, -50 dBm = 100%)
    uint8_t rssiToPercent(int8_t rssi);
    
    // RSSI zu Farbe (Rot/Gelb/Grün)
    uint16_t rssiToColor(int8_t rssi);
};

#endif // CONNECTION_PAGE_H