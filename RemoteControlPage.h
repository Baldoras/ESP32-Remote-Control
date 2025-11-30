/**
 * RemoteControlPage.h
 * 
 * Hauptseite der Fernsteuerung mit Joystick-Visualisierung
 * 
 * Features:
 * - Joystick-Position Visualisierung (Kreuz + Punkt)
 * - Motor-Output Balken (Links/Rechts)
 * - Batterie-Status (Remote + Fahrwerk)
 * - Verbindungsstatus-Icon
 * - RSSI-Indikator
 * - Sende-Rate Anzeige
 */

#ifndef REMOTE_CONTROL_PAGE_H
#define REMOTE_CONTROL_PAGE_H

#include "UIPage.h"
#include "UILabel.h"
#include "UIButton.h"
#include "UIProgressBar.h"
#include "ESPNowManager.h"

class RemoteControlPage : public UIPage {
public:
    /**
     * Konstruktor
     * @param ui UIManager Pointer
     * @param tft TFT Display Pointer
     */
    RemoteControlPage(UIManager* ui, TFT_eSPI* tft);
    
    /**
     * Destruktor
     */
    ~RemoteControlPage();

    /**
     * Seite erstellen (von UIPage geerbt)
     */
    void build() override;

    /**
     * Update (in loop() aufrufen)
     */
    void update() override;

    /**
     * Joystick-Position setzen (für Visualisierung)
     * @param x X-Achse (-100 bis +100)
     * @param y Y-Achse (-100 bis +100)
     */
    void setJoystickPosition(int16_t x, int16_t y);

    /**
     * Motor-Output setzen (vom Fahrwerk empfangen)
     * @param left Links (-100 bis +100)
     * @param right Rechts (-100 bis +100)
     */
    void setMotorOutput(int16_t left, int16_t right);

    /**
     * Batterie-Status setzen (Remote)
     * @param voltage Spannung in Volt
     * @param percent Prozent (0-100)
     */
    void setRemoteBattery(float voltage, uint8_t percent);

    /**
     * Batterie-Status setzen (Fahrwerk, via ESP-NOW)
     * @param voltage Spannung in Volt
     * @param percent Prozent (0-100)
     */
    void setVehicleBattery(float voltage, uint8_t percent);

    /**
     * Verbindungsstatus aktualisieren
     * @param connected Verbunden?
     */
    void setConnectionStatus(bool connected);

    /**
     * RSSI aktualisieren
     * @param rssi Signalstärke in dBm
     */
    void setRSSI(int8_t rssi);

    /**
     * Sende-Rate aktualisieren
     * @param packetsPerSecond Pakete/Sekunde
     */
    void setSendRate(uint16_t packetsPerSecond);

private:
    // ESP-NOW Manager Referenz
    EspNowManager& espnow;
    
    // Joystick-Daten
    int16_t joystickX;          // -100 bis +100
    int16_t joystickY;          // -100 bis +100
    
    // Motor-Daten
    int16_t motorLeft;          // -100 bis +100
    int16_t motorRight;         // -100 bis +100
    
    // Batterie-Daten
    float remoteBatteryVoltage;
    uint8_t remoteBatteryPercent;
    float vehicleBatteryVoltage;
    uint8_t vehicleBatteryPercent;
    
    // Verbindungs-Daten
    bool isConnected;
    int8_t currentRSSI;
    uint16_t sendRate;
    
    // Joystick-Visualisierung Bereich
    int16_t joystickAreaX;
    int16_t joystickAreaY;
    int16_t joystickAreaSize;
    int16_t joystickCenterX;
    int16_t joystickCenterY;
    
    // UI-Elemente
    // Status-Bar (oben)
    UILabel* labelConnectionStatus;
    UILabel* labelRSSI;
    UILabel* labelSendRate;
    
    // Joystick-Werte
    UILabel* labelJoystickX;
    UILabel* labelJoystickY;
    
    // Motor-Balken
    UILabel* labelMotorLeft;
    UIProgressBar* barMotorLeft;
    UILabel* labelMotorLeftValue;
    
    UILabel* labelMotorRight;
    UIProgressBar* barMotorRight;
    UILabel* labelMotorRightValue;
    
    // Batterie-Anzeigen
    UILabel* labelRemoteBattery;
    UIProgressBar* barRemoteBattery;
    UILabel* labelRemoteBatteryValue;
    
    UILabel* labelVehicleBattery;
    UIProgressBar* barVehicleBattery;
    UILabel* labelVehicleBatteryValue;
    
    // Buttons
    UIButton* btnSettings;
    
    // Interne Methoden
    void initUI();
    void drawJoystickArea();
    void drawJoystickPosition();
    void updateMotorBars();
    void updateBatteryBars();
    void updateStatusBar();
    
    // Helper
    int16_t mapJoystickToPixel(int16_t value, int16_t center, int16_t radius);
    uint16_t percentToColor(uint8_t percent);  // Rot -> Gelb -> Grün
    uint16_t rssiToColor(int8_t rssi);
};

#endif // REMOTE_CONTROL_PAGE_H
