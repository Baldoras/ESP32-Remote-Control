/**
 * UIPageManager.cpp
 */

#include "UIPageManager.h"

UIPageManager::UIPageManager()
    : currentPageId(-1), currentPageIndex(-1) {
}

UIPageManager::~UIPageManager() {
    // Seiten werden nicht gelöscht (Ownership beim Aufrufer)
    pages.clear();
}

bool UIPageManager::addPage(UIPage* page, int pageId) {
    if (!page) return false;
    
    // Prüfen ob ID bereits existiert
    if (findPageIndex(pageId) != -1) {
        Serial.printf("UIPageManager: Seiten-ID %d existiert bereits!\n", pageId);
        return false;
    }
    
    PageEntry entry;
    entry.page = page;
    entry.pageId = pageId;
    
    pages.push_back(entry);
    
    // NICHT automatisch erste Seite anzeigen!
    // Seiten werden erst bei explizitem showPage() angezeigt
    
    return true;
}

bool UIPageManager::showPage(int pageId) {
    int index = findPageIndex(pageId);
    
    if (index == -1) {
        Serial.printf("UIPageManager: Seite mit ID %d nicht gefunden!\n", pageId);
        return false;
    }
    
    // Aktuelle Seite verstecken
    if (currentPageIndex != -1 && currentPageIndex < pages.size()) {
        pages[currentPageIndex].page->hide();
    }
    
    // Neue Seite anzeigen
    currentPageIndex = index;
    currentPageId = pageId;
    pages[currentPageIndex].page->show();
    
    Serial.printf("UIPageManager: Zeige Seite '%s' (ID=%d)\n", 
                  pages[currentPageIndex].page->getPageName(), pageId);
    
    return true;
}

bool UIPageManager::showPageByIndex(int index) {
    if (index < 0 || index >= pages.size()) {
        return false;
    }
    
    return showPage(pages[index].pageId);
}

void UIPageManager::nextPage() {
    if (pages.empty()) return;
    
    int nextIndex = (currentPageIndex + 1) % pages.size();
    showPageByIndex(nextIndex);
}

void UIPageManager::previousPage() {
    if (pages.empty()) return;
    
    int prevIndex = currentPageIndex - 1;
    if (prevIndex < 0) prevIndex = pages.size() - 1;
    
    showPageByIndex(prevIndex);
}

UIPage* UIPageManager::getCurrentPage() {
    if (currentPageIndex >= 0 && currentPageIndex < pages.size()) {
        return pages[currentPageIndex].page;
    }
    return nullptr;
}

UIPage* UIPageManager::getPage(int pageId) {
    int index = findPageIndex(pageId);
    if (index != -1) {
        return pages[index].page;
    }
    return nullptr;
}

void UIPageManager::update() {
    UIPage* currentPage = getCurrentPage();
    if (currentPage && currentPage->isVisible()) {
        currentPage->update();
    }
}

int UIPageManager::findPageIndex(int pageId) {
    for (int i = 0; i < pages.size(); i++) {
        if (pages[i].pageId == pageId) {
            return i;
        }
    }
    return -1;
}