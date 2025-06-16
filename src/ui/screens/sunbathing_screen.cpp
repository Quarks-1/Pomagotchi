#include "ui/logged_message.h"
#include "ui/screens/screens.h"
#include "assets/sprites.h"
#include "pet/pet_state.h"
#include "animation/animation.h"
#include "ui/ui_components.h"
#include "ui/ui_config.h"
#include "ui/cursor.h"
#include "activities/sunbathing.h"
#include "activities/water_logging.h"

void drawSunbathingPage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    static AnimationState petAnimation = {AnimationType::SIT, 0, 0, true};
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
    }

    // Update animation state
    updateAnimation(petAnimation);
    
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
    
    // Try to draw sprite with equipped hat (clears combined area and draws both)
    if (!drawEquippedHatOnSprite(display, 0, 75, currentFrame)) {
        // No hat equipped, draw sprite normally
        drawAnimationFrame(display, 0, 75, currentFrame);
    }

    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 