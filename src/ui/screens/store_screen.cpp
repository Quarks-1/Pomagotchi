#include "ui/screens/screens.h"
#include "assets/sprites.h"
#include "pet/pet_state.h"
#include "animation/animation.h"
#include "ui/ui_components.h"
#include "ui/ui_config.h"
#include "ui/cursor.h"

void drawStorePage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    static AnimationState petAnimation = {AnimationType::IDLE, 0, 0, true};
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
    }
    
    // Update animation state
    updateAnimation(petAnimation);
    
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    
    drawTitle(display, 45, 20, "Store");
    drawStarCluster(display, 0, 0, 32, 32);
    
    // Draw star counter
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    // Clear the area for the number
    display.fillRect(115, 5, 25, 20, GxEPD_WHITE);
    display.setCursor(115, 20);
    display.print(getStars());
    drawStarCluster(display, 140, 5, 16, 16);
    
    drawStarCluster(display, 168, 0, 32, 32);

    // Change screen buttons with cursor selection
    drawChangeScreenButton(display, 10, 170, 85, 30, "left", cursorPosition == 0, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 90, 170, 30, 30, "up", cursorPosition == 1, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 115, 170, 85, 30, "right", cursorPosition == 2, 3, 3, UIConfig::CORNER_RADIUS);

    // Draw current animation frame
    const AnimationFrame& currentFrame = getCurrentFrame(petAnimation);
    drawAnimationFrame(display, 0, 75, currentFrame);

    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 