/**
 * RemoteControlPage.cpp
 * 
 * Implementation der Fernsteuerungs-Seite
 */

#include "RemoteControlPage.h"

RemoteControlPage::RemoteControlPage(UIManager* ui, TFT_eSPI* tft)
    : UIPage("Remote Control", ui, tft)
    , espnow(EspNowManager::getInstance())
    , joystickX(0)
    , joystickY(0)
    , motorLeft(0)
    , motorRight(0)
    , remoteBatteryVoltage(0.0f)
    , remoteBatteryPercent(0)
    , vehicleBatteryVoltage(0.0f)
    , vehicleBatteryPercent(0)
    , isConnected(false)
    , currentRSSI(-100)
    , sendRate(0)
    , joystickAreaX(0)
    , joystickAreaY(0)
    , joystickAreaSize(0)
    , joystickCenterX(0)
    , joystickCenterY(0)
{
    // Back-Button zu Home
    // Wird in initUI() erstellt
}

RemoteControlPage::~RemoteControlPage() {
    // UI-Elemente werden von UIPage aufgeräumt
}

void RemoteControlPage::build() {
    DEBUG_PRINTLN("RemoteControlPage: Building UI...");
    
    initUI();
    
    DEBUG_PRINTLN("RemoteControlPage: ✅ UI erstellt");
}

void RemoteControlPage::update() {
    // Joystick-Position neu zeichnen (wenn geändert)
    static int16_t lastJoyX = 0;
    static int16_t lastJoyY = 0;
    
    if (joystickX != lastJoyX || joystickY != lastJoyY) {
        drawJoystickPosition();
        lastJoyX = joystickX;
        lastJoyY = joystickY;
        
        // Werte-Labels aktualisieren
        char buffer[16];
        sprintf(buffer, "X: %d", joystickX);
        labelJoystickX->setText(buffer);
        
        sprintf(buffer, "Y: %d", joystickY);
        labelJoystickY->setText(buffer);
    }
    
    // Status-Bar aktualisieren
    static unsigned long lastStatusUpdate = 0;
    if (millis() - lastStatusUpdate > 500) {
        updateStatusBar();
        lastStatusUpdate = millis();
    }
}

void RemoteControlPage::setJoystickPosition(int16_t x, int16_t y) {
    joystickX = constrain(x, -100, 100);
    joystickY = constrain(y, -100, 100);
}

void RemoteControlPage::setMotorOutput(int16_t left, int16_t right) {
    motorLeft = constrain(left, -100, 100);
    motorRight = constrain(right, -100, 100);
    updateMotorBars();
}

void RemoteControlPage::setRemoteBattery(float voltage, uint8_t percent) {
    remoteBatteryVoltage = voltage;
    remoteBatteryPercent = constrain(percent, 0, 100);
    updateBatteryBars();
}

void RemoteControlPage::setVehicleBattery(float voltage, uint8_t percent) {
    vehicleBatteryVoltage = voltage;
    vehicleBatteryPercent = constrain(percent, 0, 100);
    updateBatteryBars();
}

void RemoteControlPage::setConnectionStatus(bool connected) {
    isConnected = connected;
    updateStatusBar();
}

void RemoteControlPage::setRSSI(int8_t rssi) {
    currentRSSI = rssi;
}

void RemoteControlPage::setSendRate(uint16_t packetsPerSecond) {
    sendRate = packetsPerSecond;
}

// ═══════════════════════════════════════════════════════════════════════════
// PRIVATE METHODEN
// ═══════════════════════════════════════════════════════════════════════════

void RemoteControlPage::initUI() {
    int16_t contentX = layout.contentX;
    int16_t contentY = layout.contentY;
    int16_t contentW = layout.contentWidth;
    int16_t contentH = layout.contentHeight;
    
    // ═══════════════════════════════════════════════════════════════════════
    // Status-Bar (oben im Content-Bereich)
    // ═══════════════════════════════════════════════════════════════════════
    
    int16_t statusY = contentY + 5;
    
    labelConnectionStatus = new UILabel(contentX + 10, statusY, 150, 25, "● DISCONNECTED");
    labelConnectionStatus->setAlignment(TextAlignment::LEFT);
    labelConnectionStatus->setFontSize(1);
    labelConnectionStatus->setTextColor(COLOR_RED);
    labelConnectionStatus->setTransparent(true);
    addContentElement(labelConnectionStatus);
    
    labelRSSI = new UILabel(contentX + 170, statusY, 120, 25, "RSSI: --- dBm");
    labelRSSI->setAlignment(TextAlignment::LEFT);
    labelRSSI->setFontSize(1);
    labelRSSI->setTransparent(true);
    addContentElement(labelRSSI);
    
    labelSendRate = new UILabel(contentX + 300, statusY, 150, 25, "Rate: 0 pkt/s");
    labelSendRate->setAlignment(TextAlignment::LEFT);
    labelSendRate->setFontSize(1);
    labelSendRate->setTransparent(true);
    addContentElement(labelSendRate);
    
    // ═══════════════════════════════════════════════════════════════════════
    // Joystick-Visualisierung (links)
    // ═══════════════════════════════════════════════════════════════════════
    
    int16_t joyStartY = contentY + 40;
    joystickAreaSize = 180;  // 180x180 Pixel
    joystickAreaX = contentX + 20;
    joystickAreaY = joyStartY;
    joystickCenterX = joystickAreaX + joystickAreaSize / 2;
    joystickCenterY = joystickAreaY + joystickAreaSize / 2;
    
    // Joystick wird direkt auf TFT gezeichnet (keine UI-Elemente)
    
    // Joystick-Werte darunter
    int16_t joyValuesY = joystickAreaY + joystickAreaSize + 10;
    
    labelJoystickX = new UILabel(joystickAreaX, joyValuesY, joystickAreaSize / 2 - 5, 25, "X: 0");
    labelJoystickX->setAlignment(TextAlignment::CENTER);
    labelJoystickX->setFontSize(1);
    labelJoystickX->setTransparent(true);
    addContentElement(labelJoystickX);
    
    labelJoystickY = new UILabel(joystickAreaX + joystickAreaSize / 2 + 5, joyValuesY, 
                                  joystickAreaSize / 2 - 5, 25, "Y: 0");
    labelJoystickY->setAlignment(TextAlignment::CENTER);
    labelJoystickY->setFontSize(1);
    labelJoystickY->setTransparent(true);
    addContentElement(labelJoystickY);
    
    // ═══════════════════════════════════════════════════════════════════════
    // Motor-Balken (rechts)
    // ═══════════════════════════════════════════════════════════════════════
    
    int16_t motorX = joystickAreaX + joystickAreaSize + 30;
    int16_t motorY = joyStartY;
    int16_t motorWidth = contentW - (motorX - contentX) - 20;
    
    // Motor Links
    labelMotorLeft = new UILabel(motorX, motorY, 100, 25, "Motor L:");
    labelMotorLeft->setAlignment(TextAlignment::LEFT);
    labelMotorLeft->setFontSize(1);
    labelMotorLeft->setTransparent(true);
    addContentElement(labelMotorLeft);
    
    motorY += 30;
    
    barMotorLeft = new UIProgressBar(motorX, motorY, motorWidth - 80, 25);
    barMotorLeft->setValue(50);  // 50% = 0 (Motor aus)
    barMotorLeft->setBarColor(COLOR_GREEN);
    barMotorLeft->setShowText(false);
    addContentElement(barMotorLeft);
    
    labelMotorLeftValue = new UILabel(motorX + motorWidth - 70, motorY, 70, 25, "0");
    labelMotorLeftValue->setAlignment(TextAlignment::CENTER);
    labelMotorLeftValue->setFontSize(1);
    labelMotorLeftValue->setTransparent(true);
    addContentElement(labelMotorLeftValue);
    
    motorY += 40;
    
    // Motor Rechts
    labelMotorRight = new UILabel(motorX, motorY, 100, 25, "Motor R:");
    labelMotorRight->setAlignment(TextAlignment::LEFT);
    labelMotorRight->setFontSize(1);
    labelMotorRight->setTransparent(true);
    addContentElement(labelMotorRight);
    
    motorY += 30;
    
    barMotorRight = new UIProgressBar(motorX, motorY, motorWidth - 80, 25);
    barMotorRight->setValue(50);
    barMotorRight->setBarColor(COLOR_GREEN);
    barMotorRight->setShowText(false);
    addContentElement(barMotorRight);
    
    labelMotorRightValue = new UILabel(motorX + motorWidth - 70, motorY, 70, 25, "0");
    labelMotorRightValue->setAlignment(TextAlignment::CENTER);
    labelMotorRightValue->setFontSize(1);
    labelMotorRightValue->setTransparent(true);
    addContentElement(labelMotorRightValue);
    
    motorY += 50;
    
    // ═══════════════════════════════════════════════════════════════════════
    // Batterie-Anzeigen (unten)
    // ═══════════════════════════════════════════════════════════════════════
    
    int16_t batteryY = contentY + contentH - 90;
    int16_t batteryWidth = (contentW - 40) / 2;
    
    // Remote-Batterie
    labelRemoteBattery = new UILabel(contentX + 10, batteryY, 150, 25, "Remote Battery:");
    labelRemoteBattery->setAlignment(TextAlignment::LEFT);
    labelRemoteBattery->setFontSize(1);
    labelRemoteBattery->setTransparent(true);
    addContentElement(labelRemoteBattery);
    
    batteryY += 25;
    
    barRemoteBattery = new UIProgressBar(contentX + 10, batteryY, batteryWidth - 80, 25);
    barRemoteBattery->setValue(0);
    barRemoteBattery->setBarColor(COLOR_RED);
    barRemoteBattery->setShowText(false);
    addContentElement(barRemoteBattery);
    
    labelRemoteBatteryValue = new UILabel(contentX + 10 + batteryWidth - 70, batteryY, 70, 25, "0.0V");
    labelRemoteBatteryValue->setAlignment(TextAlignment::CENTER);
    labelRemoteBatteryValue->setFontSize(1);
    labelRemoteBatteryValue->setTransparent(true);
    addContentElement(labelRemoteBatteryValue);
    
    batteryY -= 25;
    
    // Vehicle-Batterie
    int16_t vehBattX = contentX + 10 + batteryWidth + 20;
    
    labelVehicleBattery = new UILabel(vehBattX, batteryY, 150, 25, "Vehicle Battery:");
    labelVehicleBattery->setAlignment(TextAlignment::LEFT);
    labelVehicleBattery->setFontSize(1);
    labelVehicleBattery->setTransparent(true);
    addContentElement(labelVehicleBattery);
    
    batteryY += 25;
    
    barVehicleBattery = new UIProgressBar(vehBattX, batteryY, batteryWidth - 80, 25);
    barVehicleBattery->setValue(0);
    barVehicleBattery->setBarColor(COLOR_RED);
    barVehicleBattery->setShowText(false);
    addContentElement(barVehicleBattery);
    
    labelVehicleBatteryValue = new UILabel(vehBattX + batteryWidth - 70, batteryY, 70, 25, "0.0V");
    labelVehicleBatteryValue->setAlignment(TextAlignment::CENTER);
    labelVehicleBatteryValue->setFontSize(1);
    labelVehicleBatteryValue->setTransparent(true);
    addContentElement(labelVehicleBatteryValue);
}

void RemoteControlPage::drawJoystickArea() {
    if (!tft) return;
    
    // Hintergrund löschen
    tft->fillRect(joystickAreaX, joystickAreaY, joystickAreaSize, joystickAreaSize, COLOR_BLACK);
    
    // Äußerer Kreis (Begrenzung)
    int16_t radius = joystickAreaSize / 2 - 5;
    tft->drawCircle(joystickCenterX, joystickCenterY, radius, COLOR_WHITE);
    tft->drawCircle(joystickCenterX, joystickCenterY, radius - 1, COLOR_WHITE);
    
    // Mittelkreuz
    int16_t crossSize = 10;
    tft->drawLine(joystickCenterX - crossSize, joystickCenterY, 
                  joystickCenterX + crossSize, joystickCenterY, COLOR_GRAY);
    tft->drawLine(joystickCenterX, joystickCenterY - crossSize,
                  joystickCenterX, joystickCenterY + crossSize, COLOR_GRAY);
    
    // 50%-Kreise
    int16_t radius50 = radius / 2;
    tft->drawCircle(joystickCenterX, joystickCenterY, radius50, COLOR_DARKGRAY);
}

void RemoteControlPage::drawJoystickPosition() {
    if (!tft) return;
    
    // Alten Bereich neu zeichnen
    drawJoystickArea();
    
    // Neue Position berechnen
    int16_t radius = joystickAreaSize / 2 - 5;
    int16_t posX = mapJoystickToPixel(joystickX, joystickCenterX, radius - 10);
    int16_t posY = mapJoystickToPixel(-joystickY, joystickCenterY, radius - 10);  // Y invertiert
    
    // Joystick-Punkt zeichnen (mit Schatten für 3D-Effekt)
    tft->fillCircle(posX + 1, posY + 1, 8, COLOR_DARKGRAY);  // Schatten
    tft->fillCircle(posX, posY, 8, COLOR_BLUE);              // Hauptpunkt
    tft->fillCircle(posX - 2, posY - 2, 3, COLOR_CYAN);      // Highlight
}

void RemoteControlPage::updateMotorBars() {
    if (!barMotorLeft || !barMotorRight) return;
    
    // Motor-Werte: -100 bis +100 → 0% bis 100% (50% = Motor aus)
    uint8_t leftPercent = (motorLeft + 100) / 2;
    uint8_t rightPercent = (motorRight + 100) / 2;
    
    barMotorLeft->setValue(leftPercent);
    barMotorRight->setValue(rightPercent);
    
    // Farbe basierend auf Richtung
    if (motorLeft > 10) {
        barMotorLeft->setBarColor(COLOR_GREEN);
    } else if (motorLeft < -10) {
        barMotorLeft->setBarColor(COLOR_ORANGE);
    } else {
        barMotorLeft->setBarColor(COLOR_GRAY);
    }
    
    if (motorRight > 10) {
        barMotorRight->setBarColor(COLOR_GREEN);
    } else if (motorRight < -10) {
        barMotorRight->setBarColor(COLOR_ORANGE);
    } else {
        barMotorRight->setBarColor(COLOR_GRAY);
    }
    
    // Werte-Text
    char buffer[16];
    sprintf(buffer, "%d", motorLeft);
    labelMotorLeftValue->setText(buffer);
    
    sprintf(buffer, "%d", motorRight);
    labelMotorRightValue->setText(buffer);
}

void RemoteControlPage::updateBatteryBars() {
    if (!barRemoteBattery || !barVehicleBattery) return;
    
    // Remote-Batterie
    barRemoteBattery->setValue(remoteBatteryPercent);
    barRemoteBattery->setBarColor(percentToColor(remoteBatteryPercent));
    
    char buffer[16];
    sprintf(buffer, "%.1fV", remoteBatteryVoltage);
    labelRemoteBatteryValue->setText(buffer);
    
    // Vehicle-Batterie
    barVehicleBattery->setValue(vehicleBatteryPercent);
    barVehicleBattery->setBarColor(percentToColor(vehicleBatteryPercent));
    
    sprintf(buffer, "%.1fV", vehicleBatteryVoltage);
    labelVehicleBatteryValue->setText(buffer);
}

void RemoteControlPage::updateStatusBar() {
    if (!labelConnectionStatus || !labelRSSI || !labelSendRate) return;
    
    // Verbindungsstatus
    if (isConnected) {
        labelConnectionStatus->setText("● CONNECTED");
        labelConnectionStatus->setTextColor(COLOR_GREEN);
    } else {
        labelConnectionStatus->setText("● DISCONNECTED");
        labelConnectionStatus->setTextColor(COLOR_RED);
    }
    
    // RSSI
    char buffer[32];
    sprintf(buffer, "RSSI: %d dBm", currentRSSI);
    labelRSSI->setText(buffer);
    labelRSSI->setTextColor(rssiToColor(currentRSSI));
    
    // Sende-Rate
    sprintf(buffer, "Rate: %d pkt/s", sendRate);
    labelSendRate->setText(buffer);
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPER FUNKTIONEN
// ═══════════════════════════════════════════════════════════════════════════

int16_t RemoteControlPage::mapJoystickToPixel(int16_t value, int16_t center, int16_t radius) {
    // -100 bis +100 → center ± radius
    return center + (value * radius) / 100;
}

uint16_t RemoteControlPage::percentToColor(uint8_t percent) {
    if (percent >= 80) return COLOR_GREEN;
    if (percent >= 50) return COLOR_YELLOW;
    if (percent >= 20) return COLOR_ORANGE;
    return COLOR_RED;
}

uint16_t RemoteControlPage::rssiToColor(int8_t rssi) {
    if (rssi >= -60) return COLOR_GREEN;
    if (rssi >= -70) return COLOR_YELLOW;
    if (rssi >= -80) return COLOR_ORANGE;
    return COLOR_RED;
}
