#include "../screens.h"
#include "../sprites.h"
#include "../pet_state.h"
#include "../animation.h"
#include "../ui_components.h"
#include "../ui_config.h"
#include "../cursor.h"

void drawFeedingPage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    static AnimationState petAnimation = {AnimationType::SNIFF, 0, 0};
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
    }

    // Update animation state
    updateAnimation(petAnimation);
    
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    
    drawTitle(display, 60, 20, "Feed Pet");
    drawStarCluster(display, 0, 0, 32, 32);
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