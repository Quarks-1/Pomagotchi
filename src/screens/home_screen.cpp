#include "../screens.h"
#include "../sprites.h"
#include "../pet_state.h"

// Removed unused global animation state
// static AnimationState petAnimation;

void drawHomePage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    
    // Idle animation state
    static int idleFrame = 0;
    static unsigned long lastFrameTime = 0;
    const int idleFrameCount = 6;
    const int idleFrameDelay = 300; // ms per frame
    const uint8_t* idleFrames[idleFrameCount] = { idle1_0, idle1_1, idle1_2, idle1_3, idle1_4, idle1_5 };

    // Get current time (replace with millis() or appropriate function for your platform)
    unsigned long now = millis();

    if (now - lastFrameTime > idleFrameDelay) {
        idleFrame = (idleFrame + 1) % idleFrameCount;
        lastFrameTime = now;
    }
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
        // Initialize animation (not used for idle now)
        // initAnimation(petAnimation, ANIM_RUN);
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
    
    // Draw idle animation frame
    int x = 92;
    int y = 100;
    int w = idleFrames[idleFrame][0];
    int h = idleFrames[idleFrame][1];
    
    // Clear the animation area before drawing the new frame
    display.fillRect(x, y, w, h, GxEPD_WHITE);
    
    display.drawBitmap(x, y, &idleFrames[idleFrame][2], w, h, GxEPD_BLACK);
    
    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 