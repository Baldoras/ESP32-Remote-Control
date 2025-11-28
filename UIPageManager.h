/**
 * UIPageManager.h
 * 
 * Verwaltet mehrere UI-Seiten und Navigation zwischen Seiten
 */

#ifndef UI_PAGE_MANAGER_H
#define UI_PAGE_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "UIPage.h"

class UIPageManager {
public:
    /**
     * Konstruktor
     */
    UIPageManager();
    
    /**
     * Destruktor
     */
    ~UIPageManager();

    /**
     * Seite hinzufügen
     * @param page UIPage Pointer
     * @param pageId Eindeutige ID für die Seite
     * @return true bei Erfolg
     */
    bool addPage(UIPage* page, int pageId);

    /**
     * Zu Seite wechseln (nach ID)
     * @param pageId Seiten-ID
     * @return true bei Erfolg
     */
    bool showPage(int pageId);

    /**
     * Zu Seite wechseln (nach Index)
     * @param index Seiten-Index
     * @return true bei Erfolg
     */
    bool showPageByIndex(int index);

    /**
     * Nächste Seite anzeigen
     */
    void nextPage();

    /**
     * Vorherige Seite anzeigen
     */
    void previousPage();

    /**
     * Aktuelle Seite abrufen
     */
    UIPage* getCurrentPage();

    /**
     * Seite nach ID abrufen
     */
    UIPage* getPage(int pageId);

    /**
     * Aktuelle Seiten-ID
     */
    int getCurrentPageId() const { return currentPageId; }

    /**
     * Anzahl der Seiten
     */
    int getPageCount() const { return pages.size(); }

    /**
     * Update (ruft update() der aktuellen Seite auf)
     */
    void update();

private:
    struct PageEntry {
        UIPage* page;
        int pageId;
    };
    
    std::vector<PageEntry> pages;
    int currentPageId;
    int currentPageIndex;
    
    /**
     * Seiten-Index nach ID finden
     */
    int findPageIndex(int pageId);
};

#endif // UI_PAGE_MANAGER_H