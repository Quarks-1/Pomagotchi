#include "ui/logged_message.h"
#include "ui/screens/screens.h"
#include "assets/sprites.h"
#include "pet/pet_state.h"
#include "animation/animation.h"
#include "ui/ui_components.h"
#include "ui/ui_config.h"
#include "ui/cursor.h"
#include "pet/pet_pommy.h"

void drawPetPommyPage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display) {
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
    
    drawTitle(display, 45, 20, "Pet Pommy!");
    drawStarCluster(display, 0, 0, 32, 32);
    drawStarCluster(display, 168, 0, 32, 32);

    // Draw heart icon and status bar
    drawIcon(display, 155, 40, 20, 20, IconType::HEART);
    
    // Clear area for pet count display
    display.fillRect(85, 60, 40, 50, GxEPD_WHITE);
    
    // Draw pet count (0-10 pets)
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(90, 100);
    if (isPetting) {
        display.print(getPetFillLevel());
    } else {
        display.print(petStatus);
    }
    display.setFont(&FreeMono9pt7b);
    display.setCursor(90, 115);
    display.println("pets");
    
    // Draw status bar moved to the right (scale 0-10 to 0-100 for display)
    uint8_t displayValue = isPetting ? (getPetFillLevel() * 10) : (petStatus * 10);
    drawStatusBar(display, 140, 70, 50, 80, displayValue, UIConfig::CORNER_RADIUS);

    // Draw logged message if needed
    if (shouldShowLoggedMessage()) {
        display.setTextColor(GxEPD_BLACK);
        display.setFont(&FreeMonoBold9pt7b);
        display.setCursor(10, 55);
        display.println("Pets logged!");
    }

    // Change screen buttons with cursor selection
    drawChangeScreenButton(display, 10, 170, 85, 30, "left", !isPetting && cursorPosition == 0, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 90, 170, 30, 30, "up", !isPetting && cursorPosition == 1, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 115, 170, 85, 30, "right", !isPetting && cursorPosition == 2, 3, 3, UIConfig::CORNER_RADIUS);

    // Draw current animation frame
    const AnimationFrame& currentFrame = getCurrentFrame(petAnimation);
    drawAnimationFrame(display, 0, 75, currentFrame);

    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 