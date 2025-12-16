/**
 * Globals.h
 * 
 * Globale Objekt-Definitionen
 * 
 * WICHTIG: Nur Forward Declarations hier!
 * Includes nur in Globals.cpp
 */

#ifndef GLOBALS_H
#define GLOBALS_H

// Forward Declarations (keine Includes!)
class DisplayHandler;
class PageManager;
class BatteryMonitor;
class JoystickHandler;
class SDCardHandler;
class TouchManager;
class UserConfig;
class PowerManager;
class EspNowManager;

// Globale Objekte (extern)
extern DisplayHandler display;
extern BatteryMonitor battery;
extern JoystickHandler joystick;
extern SDCardHandler sdCard;
extern TouchManager touch;
extern UserConfig userConfig;
extern PowerManager powerMgr;
extern EspNowManager espNow;

// PageManager als Pointer (wird in setup() erstellt)
extern PageManager* pageManager;

#endif // GLOBALS_H