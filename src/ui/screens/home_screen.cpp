#include "ui/screens/screens.h"
#include "assets/sprites.h"
#include "pet/pet_state.h"
#include "animation/animation.h"
#include "ui/ui_components.h"
#include "ui/ui_config.h"
#include "ui/cursor.h"
#include "system/sleep_message.h"
#include "../ui_components.h"
#include "../ui_config.h"
#include "../cursor.h"
#include "system/sleep_message.h"
#include "system/sleep_manager.h"

// Removed unused global animation state
// static AnimationState petAnimation;

void drawHomePage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
    static bool firstDraw = true;
    static AnimationState petAnimation = {AnimationType::WALK, 0, 0, false};
    
    if (firstDraw) {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        firstDraw = false;
    }
    
    // Update animation state
    updateAnimation(petAnimation);
    
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    
    drawTitle(display, 45, 20, "Pomagotchi");
    drawStarCluster(display, 0, 0, 32, 32);
    drawStarCluster(display, 168, 0, 32, 32);
    
    // Draw icons and status bars
    
    // Sunlight
    drawIcon(display, 85, 40, 20, 20, IconType::SUN);
    drawStatusBar(display, 85, 70, 20, 80, sunlight, UIConfig::CORNER_RADIUS);
    
    // Thirst
    drawIcon(display, 115, 40, 20, 20, IconType::WATER_DROP);
    drawStatusBar(display, 115, 70, 20, 80, thirst, UIConfig::CORNER_RADIUS);

    // Pet Status (0-10 pets scaled to 0-100 for display)
    drawIcon(display, 145, 40, 20, 20, IconType::HEART);
    drawStatusBar(display, 145, 70, 20, 80, petStatus * 10, UIConfig::CORNER_RADIUS);

    // Battery
    drawIcon(display, 175, 40, 20, 20, IconType::LIGHTNING);
    drawStatusBar(display, 175, 70, 20, 80, getBatteryLevel(), UIConfig::CORNER_RADIUS);

    // Draw sleeping message if needed
    if (shouldShowSleepMessage() || sleepManager.isInLightSleep()) {
        display.setTextColor(GxEPD_BLACK);
        display.setFont(&FreeMonoBold9pt7b);
        display.setCursor(5, 55);
        display.println("Zzzzz...");
    }

    // Change screen buttons with cursor selection
    drawChangeScreenButton(display, 10, 170, 90, 30, "left", cursorPosition == 0, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 110, 170, 90, 30, "right", cursorPosition == 1, 3, 3, UIConfig::CORNER_RADIUS);
    
    // Draw pet sprite - use sniff_0 when entering sleep mode, otherwise use normal animation
    if (shouldShowSleepMessage() || sleepManager.isInLightSleep()) {
        // Create frame for sleeping sprite
        AnimationFrame sleepFrame = {sniff_0, sniff_0[0], sniff_0[1]};
        
        // Try to draw sprite with equipped hat (clears combined area and draws both)
        if (!drawEquippedHatOnSprite(display, -5, 75, sleepFrame)) {
            // No hat equipped, draw sprite normally
            display.fillRect(-5, 75, sniff_0[0], sniff_0[1] + 3, GxEPD_WHITE);
            display.drawBitmap(-5, 75, &sniff_0[2], sniff_0[0], sniff_0[1], GxEPD_BLACK);
        }
    } else {
        // Get current animation frame
        const AnimationFrame& currentFrame = getCurrentFrame(petAnimation);
        
        // Try to draw sprite with equipped hat (clears combined area and draws both)
        if (!drawEquippedHatOnSprite(display, -5, 75, currentFrame)) {
            // No hat equipped, draw sprite normally
            drawAnimationFrame(display, -5, 75, currentFrame);
        }
    }
    
    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 