/**
 * HomePage.h
 * 
 * Startseite mit Navigation zu allen anderen Pages
 */

#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "../ui/core/UIPage.h"
#include "../ui/core/UIManager.h"
#include <TFT_eSPI.h>

class HomePage : public UIPage {
public:
    HomePage(UIManager* ui, TFT_eSPI* tft);
    
    void build() override;
    void update() override;
    
private:
    void updateStatus();
};

#endif // HOMEPAGE_H