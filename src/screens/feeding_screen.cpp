#include "../screens.h"
#include "../sprites.h"

void drawFeedingPage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
    }
    
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    
    // Title
    display.setCursor(10, 20);
    display.println("Feed Pet");
    
    // Instructions
    display.setCursor(10, 50);
    display.println("Press button to");
    display.setCursor(10, 70);
    display.println("feed your pet");
    
    // Draw food bowl sprite in the center
    display.drawBitmap(92, 100, foodBowlBitmap, 16, 16, GxEPD_BLACK);
    
    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 