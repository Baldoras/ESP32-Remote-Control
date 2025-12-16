/**
 * Globals.cpp
 * 
 * Globale Objekt-Instanzen
 * Alle Includes hier (nicht in Globals.h!)
 * 
 * ACHTUNG: PageManager benötigt Konstruktor-Parameter!
 * Wird in setup() initialisiert, hier nur Platzhalter
 */

#include "Globals.h"
#include "../hardware/DisplayHandler.h"
#include "PageManager.h"
#include "../hardware/BatteryMonitor.h"
#include "../hardware/JoystickHandler.h"
#include "../hardware/SDCardHandler.h"
#include "../hardware/TouchManager.h"
#include "UserConfig.h"
#include "../hardware/PowerManager.h"
#include "../hardware/ESPNowManager.h"

// Globale Instanzen
DisplayHandler display;
BatteryMonitor battery;
JoystickHandler joystick;
SDCardHandler sdCard;
TouchManager touch;
UserConfig userConfig;
PowerManager powerMgr;
EspNowManager espNow;

// PageManager als Pointer (wird in setup() erstellt)
PageManager* pageManager = nullptr;