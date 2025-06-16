#include "../logged_message.h"
#include "ui/screens/screens.h"
#include "assets/sprites.h"
#include "assets/hat_sprites.h"
#include "pet/pet_state.h"
#include "animation/animation.h"
#include "ui/ui_components.h"
#include "ui/ui_config.h"
#include "ui/cursor.h"
#include "../activities/store_purchasing.h"
#include <string> 


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
    display.fillRect(112, 5, 43, 20, GxEPD_WHITE);
    display.setCursor(112, 20);
    display.print(getStars());
    drawSingleStar(display, 155, 15, 8);  // Small star next to counter
    
    drawStarCluster(display, 168, 0, 32, 32);

    // Draw current animation frame (left side)
    const AnimationFrame& currentFrame = getCurrentFrame(petAnimation);
    
    // Try to draw sprite with equipped hat (clears combined area and draws both)
    if (!drawEquippedHatOnSprite(display, -5, 75, currentFrame)) {
        // No hat equipped, draw sprite normally
        drawAnimationFrame(display, -5, 75, currentFrame);
    }

    // Carousel interface to the right of the animation (always visible)
    // Clear the area first
    display.fillRect(70, 40, 130, 120, GxEPD_WHITE);
    
    // Hat name and price on the same line
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(75, 55);
    display.print(HAT_NAMES[selectedHat]);
    display.print(" ");
    display.print(HAT_PRICES[selectedHat]);
    // Draw single star next to price instead of "stars"
    drawSingleStar(display, 75 + (strlen(HAT_NAMES[selectedHat]) + 1 + std::to_string(HAT_PRICES[selectedHat]).length()) * 11 + 7, 50, 8);
    
    // Clear status message area to avoid overlaps
    display.fillRect(0, 30, 65, 40, GxEPD_WHITE);
    
    // Status message above the pet sprite
    display.setFont(&FreeMono9pt7b);
    display.setCursor(0, 45);
    if (isHatPurchased(selectedHat)) {
        display.print("owned!");
    } else if (canAffordHat(selectedHat)) {
        display.println("can");
        display.setCursor(0, 60);
        display.print("buy!");
    } else {
        display.println("too");
        display.setCursor(0, 60);
        display.print("poor");
    }
    
    // Hat drawing area - show bitmap sprite
    display.fillRect(75, 65, 120, 40, GxEPD_WHITE);
    display.drawRect(75, 65, 120, 40, GxEPD_BLACK);
    
    // Draw hat sprite centered in the area
    const uint8_t* hatSprite = getHatSprite(selectedHat);
    if (hatSprite != nullptr) {
        // Get sprite dimensions
        uint8_t spriteWidth = hatSprite[0];
        uint8_t spriteHeight = hatSprite[1];
        
        // Center the sprite in the 120x40 area
        int16_t spriteX = 75 + (120 - spriteWidth) / 2;
        int16_t spriteY = 65 + (40 - spriteHeight) / 2;
        
        // Draw the hat sprite
        drawHatSprite(display, spriteX, spriteY, hatSprite);
    }
    
    // Equip status box above the carousel buttons
    display.fillRect(75, 110, 120, 20, GxEPD_WHITE);
    display.drawRect(75, 110, 120, 20, GxEPD_BLACK);
    display.setFont(&FreeMono9pt7b);
    display.setCursor(80, 125);
    if (isHatEquipped(selectedHat)) {
        display.print("EQUIPPED");
    } else if (isHatPurchased(selectedHat)) {
        display.print("UNEQUIPPED");  
    } else {
        display.print("NOT OWNED");
    }
    
    // Three buttons in a row: Next Hat | Buy/Equip | Exit
    // Only highlight if in purchasing mode
    // Change button text based on hat ownership
    const char* actionButtonText = isHatPurchased(selectedHat) ? "wear" : "buy";
    drawChangeScreenButton(display, 70, 135, 40, 25, "right", isPurchasing && cursorPosition == 0, 2, 2, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 110, 135, 50, 25, actionButtonText, isPurchasing && cursorPosition == 1, 2, 2, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 160, 135, 40, 25, "down", isPurchasing && cursorPosition == 2, 2, 2, UIConfig::CORNER_RADIUS);

    drawChangeScreenButton(display, 10, 170, 85, 30, "left", !isPurchasing && cursorPosition == 0, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 90, 170, 30, 30, "up", !isPurchasing && cursorPosition == 1, 3, 3, UIConfig::CORNER_RADIUS);
    drawChangeScreenButton(display, 115, 170, 85, 30, "right", !isPurchasing && cursorPosition == 2, 3, 3, UIConfig::CORNER_RADIUS);

    display.displayWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
} 