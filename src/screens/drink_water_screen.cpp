#include "../screens.h"
#include "../sprites.h"

void drawDrinkWaterPage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
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
    display.println("Give Water");
    
    // Instructions
    display.setCursor(10, 50);
    display.println("Press button to");
    display.setCursor(10, 70);
    display.println("give water");
    
    // Draw water bottle sprite (2x2 grid for larger size)
    display.drawBitmap(150, 52, waterBottleBitmap, 8, 8, GxEPD_BLACK);
    display.drawBitmap(158, 52, waterBottleBitmap, 8, 8, GxEPD_BLACK);
    display.drawBitmap(150, 60, waterBottleBitmap, 8, 8, GxEPD_BLACK);
    display.drawBitmap(158, 60, waterBottleBitmap, 8, 8, GxEPD_BLACK);
    
    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 