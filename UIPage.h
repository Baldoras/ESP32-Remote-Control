/**
 * UIPage.h
 * 
 * Vereinfachte Basis-Klasse für UI-Seiten
 * 
 * WICHTIG: Header und Footer werden von GlobalUI verwaltet!
 * Pages verwalten nur noch den Content-Bereich (40-280px)
 * 
 * Struktur:
 * ┌─────────────────────────────┐
 * │ [<] Titel        [Batt 85%] │ ← GlobalUI (Header)
 * ├─────────────────────────────┤
 * │                             │
 * │ Content (40-280px)          │ ← UIPage verwaltet NUR diesen Bereich
 * │                             │
 * ├─────────────────────────────┤
 * │ v1.0.0 | Ready              │ ← GlobalUI (Footer)
 * └─────────────────────────────┘
 */

#ifndef UI_PAGE_H
#define UI_PAGE_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <vector>
#include "UIManager.h"
#include "UIElement.h"

// Forward declarations
class GlobalUI;
class UIPageManager;

// Page-Layout Konfiguration (nur Content-Bereich!)
struct PageLayout {
    int16_t contentX;           // Content X-Start (immer 0)
    int16_t contentY;           // Content Y-Start (immer 40)
    int16_t contentWidth;       // Content Breite (immer 480)
    int16_t contentHeight;      // Content Höhe (immer 240)
    uint16_t contentBgColor;    // Content Hintergrundfarbe
};

class UIPage {
public:
    /**
     * Konstruktor
     * @param name Seitenname (wird in GlobalUI Header angezeigt)
     * @param ui UIManager Pointer
     * @param tft TFT Display Pointer
     */
    UIPage(const char* name, UIManager* ui, TFT_eSPI* tft);
    
    /**
     * Destruktor
     */
    virtual ~UIPage();

    /**
     * Seite erstellen (wird beim ersten Anzeigen aufgerufen)
     * MUSS in abgeleiteten Klassen implementiert werden!
     * 
     * WICHTIG: Nur Content-Widgets erstellen (Y-Position >= 40)!
     */
    virtual void build() = 0;

    /**
     * Seite anzeigen
     * - Content-Bereich löschen
     * - GlobalUI aktualisieren (Titel, Zurück-Button, Battery-Icon)
     * - Content-Elemente sichtbar machen
     */
    void show();

    /**
     * Seite verstecken
     * - Content-Elemente verstecken
     */
    void hide();

    /**
     * Ist Seite sichtbar?
     */
    bool isVisible() const { return visible; }

    /**
     * Update (wird in loop() aufgerufen wenn sichtbar)
     * Kann in abgeleiteten Klassen überschrieben werden
     */
    virtual void update();

    /**
     * Seitenname abrufen
     */
    const char* getPageName() const { return pageName; }

    /**
     * Layout abrufen (Content-Bereich)
     */
    const PageLayout& getLayout() const { return layout; }

    /**
     * Zurück-Button konfigurieren
     * @param enabled Zurück-Button aktivieren?
     * @param targetPageId Ziel-Seite beim Klick
     */
    void setBackButton(bool enabled, int targetPageId = -1);

    /**
     * GlobalUI Instanz setzen (wird von Main aufgerufen)
     */
    void setGlobalUI(GlobalUI* globalUI);

    /**
     * PageManager Instanz setzen (wird von Main aufgerufen)
     */
    void setPageManager(UIPageManager* pageManager);

protected:
    UIManager* ui;              // UI-Manager
    TFT_eSPI* tft;              // Display
    GlobalUI* globalUI;         // Global UI (Header/Footer)
    UIPageManager* pageManager; // Page Manager (für Navigation)
    
    char pageName[32];          // Seitenname
    bool visible;               // Sichtbar?
    bool built;                 // Bereits gebaut?
    PageLayout layout;          // Layout-Konfiguration (Content-Bereich)
    
    // Zurück-Button Konfiguration
    bool hasBackButton;         // Zurück-Button aktiviert?
    int backButtonTarget;       // Ziel-Seite
    
    // Content-Elemente (von abgeleiteten Klassen verwaltet)
    std::vector<UIElement*> contentElements;
    
    /**
     * Element zum Content hinzufügen
     * @param element UI-Element
     */
    void addContentElement(UIElement* element);
    
    /**
     * Standard-Layout initialisieren (Content-Bereich)
     */
    void initDefaultLayout();
};

#endif // UI_PAGE_H