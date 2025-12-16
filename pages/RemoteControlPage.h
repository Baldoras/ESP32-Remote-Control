/**
 * RemoteControlPage.h
 * 
 * Fernsteuerungs-Seite mit Joystick-Visualisierung
 */

#ifndef REMOTECONTROLPAGE_H
#define REMOTECONTROLPAGE_H

#include "../ui/widgets/UIPage.h"
#include "../ui/core/UIManager.h"
#include "../ui/widgets/UILabel.h"
#include "../ui/widgets/UIProgressBar.h"
#include <TFT_eSPI.h>

class RemoteControlPage : public UIPage {
public:
    RemoteControlPage(UIManager* ui, TFT_eSPI* tft);
    
    void build() override;
    void update() override;
    
    void setJoystickPosition(int16_t x, int16_t y);
    
private:
    void drawJoystickPosition();
    
    int16_t joystickX, joystickY;
    int16_t joystickAreaX, joystickAreaY, joystickAreaSize;
    int16_t joystickCenterX, joystickCenterY;
    
    UILabel* labelConnectionStatus;
    UILabel* labelJoystickX;
    UILabel* labelJoystickY;
    UIProgressBar* barRemoteBattery;
    UILabel* labelRemoteBatteryValue;
};

#endif // REMOTECONTROLPAGE_H