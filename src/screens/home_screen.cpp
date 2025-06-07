#include "../screens.h"
#include "../sprites.h"
#include "../pet_state.h"
#include "../animation.h"

// Removed unused global animation state
// static AnimationState petAnimation;

void drawHomePage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    static AnimationState petAnimation = {AnimationType::JUMP, 0, 0};
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
    }
    
    // Update animation state
    updateAnimation(petAnimation);
    
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
    
    // Draw current animation frame
    const AnimationFrame& currentFrame = getCurrentFrame(petAnimation);
    int x = 25;
    int y = 100;
    
    // Clear the animation area before drawing the new frame
    display.fillRect(x, y, currentFrame.width, currentFrame.height, GxEPD_WHITE);
    
    display.drawBitmap(x, y, &currentFrame.data[2], currentFrame.width, currentFrame.height, GxEPD_BLACK);
    
    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 