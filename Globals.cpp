/**
 * Globals.cpp
 * 
 * Globale Objekt-Instanzen
 * Alle Includes hier (nicht in Globals.h!)
 */

#include "Globals.h"
#include "DisplayHandler.h"
#include "UIPageManager.h"
#include "BatteryMonitor.h"
#include "JoystickHandler.h"
#include "SDCardHandler.h"
#include "TouchManager.h"
#include "UserConfig.h"
#include "PowerManager.h"

// Globale Instanzen
DisplayHandler display;
UIPageManager pageManager;
BatteryMonitor battery;
JoystickHandler joystick;
SDCardHandler sdCard;
TouchManager touch;
UserConfig userConfig;
PowerManager powerMgr;