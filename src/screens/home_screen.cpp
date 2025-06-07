#include "../screens.h"
#include "../sprites.h"
#include "../pet_state.h"

void drawHomePage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    // Only clear and fill screen on first draw
    static bool firstDraw = true;
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);  // Set background to white
        firstDraw = false;
    }
    
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    
    // Draw title
    display.setCursor(10, 20);
    display.println("Pomagotchi");
    
    // Draw status
    display.setCursor(10, 50);
    display.println("Status:");
    
    // Clear and draw hunger
    display.fillRect(80, 58, 100, 15, GxEPD_WHITE);  // Clear hunger value area
    display.setCursor(10, 70);
    display.print("Hunger: ");
    display.print(hunger);
    display.println("%");
    
    // Clear and draw thirst
    display.fillRect(80, 78, 100, 15, GxEPD_WHITE);  // Clear thirst value area
    display.setCursor(10, 90);
    display.print("Thirst: ");
    display.print(thirst);
    display.println("%");
    
    // Draw pomeranian sprite in the center
    display.drawBitmap(92, 100, pomeranianBitmap, 16, 16, GxEPD_BLACK);
    
    // Use partial refresh for updates
    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 