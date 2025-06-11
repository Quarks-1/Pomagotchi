#include "../screens.h"
#include "../sprites.h"
#include "../pet_state.h"
#include "../animation.h"
#include "../ui_components.h"
#include "../ui_config.h"
#include "../cursor.h"

// Removed unused global animation state
// static AnimationState petAnimation;

void drawHomePage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    static AnimationState petAnimation = {AnimationType::WALK, 0, 0, false};
    static uint8_t lastSunlight = 0;
    static uint8_t lastThirst = 0;
    static uint8_t lastBatteryLevel = 0;
    static uint8_t lastCursorPosition = 0;
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
        // Force an update on first draw
        lastSunlight = sunlight + 1;  // Force different from current
        lastThirst = thirst + 1;  // Force different from current
        lastBatteryLevel = getBatteryLevel() + 1;  // Force different from current
        lastCursorPosition = cursorPosition + 1;  // Force different from current
    }
    
    // Update animation state
    updateAnimation(petAnimation);
    
    // Check if we need to update the display
    bool needsUpdate = firstDraw || 
                      lastSunlight != sunlight ||
                      lastThirst != thirst ||
                      lastBatteryLevel != getBatteryLevel() ||
                      lastCursorPosition != cursorPosition;
    
    if (!needsUpdate) {
        // Draw star clusters and animation frame even if nothing else needs updating
        drawStarCluster(display, 0, 0, 32, 32);
        drawStarCluster(display, 168, 0, 32, 32);
        const AnimationFrame& currentFrame = getCurrentFrame(petAnimation);
        drawAnimationFrame(display, 0, 75, currentFrame);
        display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
        return;
    }
    
    // Update our tracking variables
    lastSunlight = sunlight;
    lastThirst = thirst;
    lastBatteryLevel = getBatteryLevel();
    lastCursorPosition = cursorPosition;
    
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    
    drawTitle(display, 45, 20, "Pomagotchi");
    drawStarCluster(display, 0, 0, 32, 32);
    drawStarCluster(display, 168, 0, 32, 32);
    
    // Draw icons and status bars
    
    // Sunlight
    drawIcon(display, 100, 40, 20, 20, IconType::SUN);
    drawStatusBar(display, 100, 70, 20, 80, sunlight, UIConfig::CORNER_RADIUS);
    
    // Thirst
    drawIcon(display, 135, 40, 20, 20, IconType::WATER_DROP);
    drawStatusBar(display, 135, 70, 20, 80, thirst, UIConfig::CORNER_RADIUS);

    // Battery
    drawIcon(display, 170, 40, 20, 20, IconType::LIGHTNING);
    drawStatusBar(display, 170, 70, 20, 80, getBatteryLevel(), UIConfig::CORNER_RADIUS);

    // Change screen buttons with cursor selection
    drawChangeScreenButton(display, 10, 170, 90, 30, "left", cursorPosition == 0, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 110, 170, 90, 30, "right", cursorPosition == 1, 3, 3, UIConfig::CORNER_RADIUS);
    
    // Draw current animation frame
    const AnimationFrame& currentFrame = getCurrentFrame(petAnimation);
    drawAnimationFrame(display, 0, 75, currentFrame);
    
    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 