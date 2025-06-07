#include "../screens.h"
#include "../sprites.h"
#include "../pet_state.h"
#include "../animations.h"

// Global animation state
static AnimationState petAnimation;

void drawHomePage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
        
        // Initialize animation
        initAnimation(petAnimation, ANIM_RUN);
    }
    
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    
    // Title
    display.setCursor(10, 20);
    display.println("Pomagotchi");
    
    // Status
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
    
    // Update and draw animation
    updateAnimation(petAnimation);
    drawAnimationFrame<GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>>(display, petAnimation, 92, 100);
    
    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 