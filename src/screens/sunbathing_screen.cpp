#include "../logged_message.h"
#include "../screens.h"
#include "../sprites.h"
#include "../pet_state.h"
#include "../animation.h"
#include "../ui_components.h"
#include "../ui_config.h"
#include "../cursor.h"
#include "../sunbathing.h"
#include "../water_logging.h"

void drawSunbathingPage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    static AnimationState petAnimation = {AnimationType::SIT, 0, 0, true};
    static uint8_t lastFillLevel = 0;
    static bool lastLoggedMessage = false;
    static uint8_t lastCursorPosition = 0;
    static bool lastSunbathing = false;
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
        // Force an update on first draw
        lastFillLevel = getSunlightFillLevel() + 1;  // Force different from current
        lastLoggedMessage = !shouldShowLoggedMessage();  // Force different from current
        lastCursorPosition = cursorPosition + 1;  // Force different from current
        lastSunbathing = !isSunbathing;  // Force different from current
    }

    // Update animation state
    updateAnimation(petAnimation);
    
    // Check if we need to update the display
    bool needsUpdate = firstDraw || 
                      lastFillLevel != getSunlightFillLevel() ||
                      lastLoggedMessage != shouldShowLoggedMessage() ||
                      lastCursorPosition != cursorPosition ||
                      lastSunbathing != isSunbathing;
    
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
    lastFillLevel = getSunlightFillLevel();
    lastLoggedMessage = shouldShowLoggedMessage();
    lastCursorPosition = cursorPosition;
    lastSunbathing = isSunbathing;
    
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    
    drawTitle(display, 45, 20, "Sunbathe!");
    drawStarCluster(display, 0, 0, 32, 32);
    drawStarCluster(display, 168, 0, 32, 32);

    // Draw sun icon and status bar
    drawIcon(display, 155, 40, 20, 20, IconType::SUN);
    
    // Clear area for sunlight amount display
    display.fillRect(85, 60, 40, 50, GxEPD_WHITE);
    
    // Draw sunlight amount in minutes (stacked vertically)
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(90, 100);
    display.print(getSunlightFillLevel() * 0.1, 1);
    display.setFont(&FreeMono9pt7b);
    display.setCursor(90, 115);
    display.println("mins");
    
    // Draw status bar moved to the right
    drawStatusBar(display, 140, 70, 50, 80, getSunlightFillLevel(), UIConfig::CORNER_RADIUS);

    // Draw logged message if needed
    if (shouldShowLoggedMessage()) {
        display.setTextColor(GxEPD_BLACK);
        display.setFont(&FreeMonoBold9pt7b);
        display.setCursor(25, 55);
        display.println("Logged!");
    }

    // Change screen buttons with cursor selection
    drawChangeScreenButton(display, 10, 170, 85, 30, "left", !isSunbathing && cursorPosition == 0, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 90, 170, 30, 30, "up", !isSunbathing && cursorPosition == 1, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 115, 170, 85, 30, "right", !isSunbathing && cursorPosition == 2, 3, 3, UIConfig::CORNER_RADIUS);

    // Draw current animation frame
    const AnimationFrame& currentFrame = getCurrentFrame(petAnimation);
    drawAnimationFrame(display, 0, 75, currentFrame);

    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 