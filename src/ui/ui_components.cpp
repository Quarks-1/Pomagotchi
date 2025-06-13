#include "ui_components.h"

// Standard dimensions and styling
const int BUTTON_WIDTH = 80;
const int BUTTON_HEIGHT = 30;
const int STATUS_BAR_HEIGHT = 25;
const int STATUS_BAR_WIDTH = 120;
const int TITLE_FONT_SIZE = 2;
const int STATUS_FONT_SIZE = 1;

void drawRectangle(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, 
                  int16_t x, int16_t y, 
                  int16_t width, int16_t height,
                  uint8_t thickness,
                  bool fill) {
    // Clear area before drawing
    display.fillRect(x, y, width, height, GxEPD_WHITE);
    
    if (fill) {
        display.fillRect(x, y, width, height, GxEPD_BLACK);
    } else {
        // Draw rectangle with specified thickness
        for (uint8_t i = 0; i < thickness; i++) {
            display.drawRect(x + i, y + i, width - (i * 2), height - (i * 2), GxEPD_BLACK);
        }
    }
}

void drawTitle(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, 
              int16_t x, int16_t y, 
              const char* text) {
    // Calculate text dimensions
    int textWidth = strlen(text) * 11; // Approximate width for bold font
    int padding = 10; // Padding around text
    int boxWidth = textWidth + (padding * 2);
    int boxHeight = 25; // Fixed height for title box
    
    // Draw box using drawRectangle
    drawRectangle(display, x - padding, y - 16, boxWidth, boxHeight, 2, false);
    
    // Draw text
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(x, y);
    display.println(text);
}

void drawButton(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, 
               int16_t x, int16_t y, 
               const char* text) {
    // Clear button area
    display.fillRect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, GxEPD_WHITE);
    display.drawRect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, GxEPD_BLACK);
    
    // Draw button text
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMono9pt7b);
    
    // Center text in button
    int16_t textWidth = strlen(text) * 6; // Approximate width of text
    int16_t textX = x + (BUTTON_WIDTH - textWidth) / 2;
    int16_t textY = y + (BUTTON_HEIGHT + 8) / 2; // 8 is approximate font height
    
    display.setCursor(textX, textY);
    display.print(text);
}

void drawStatusBar(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                  int16_t x, int16_t y,
                  int16_t width, int16_t height,
                  uint8_t fill,
                  uint8_t radius) {
    // Clear the area
    display.fillRect(x, y, width, height, GxEPD_WHITE);
    
    // Draw rounded rectangle border
    for (int i = 0; i < radius; i++) {
        // Top and bottom edges
        display.drawFastHLine(x + radius, y + i, width - (radius * 2), GxEPD_BLACK);
        display.drawFastHLine(x + radius, y + height - 1 - i, width - (radius * 2), GxEPD_BLACK);
        
        // Left and right edges
        display.drawFastVLine(x + i, y + radius, height - (radius * 2), GxEPD_BLACK);
        display.drawFastVLine(x + width - 1 - i, y + radius, height - (radius * 2), GxEPD_BLACK);
        
        // Corners
        display.drawPixel(x + radius - i, y + radius - i, GxEPD_BLACK);
        display.drawPixel(x + width - radius + i, y + radius - i, GxEPD_BLACK);
        display.drawPixel(x + radius - i, y + height - radius + i, GxEPD_BLACK);
        display.drawPixel(x + width - radius + i, y + height - radius + i, GxEPD_BLACK);
    }
    
    // Calculate fill height (from bottom up)
    int fillHeight = (height * fill) / 100;
    
    // Draw fill with rounded corners
    if (fillHeight > 0) {
        // Ensure fill doesn't exceed the rounded corners
        if (fillHeight < radius * 2) {
            fillHeight = radius * 2;
        }
        
        // Fill the main rectangle
        display.fillRect(x + 1, y + height - fillHeight, width - 2, fillHeight - 1, GxEPD_BLACK);
        
        // Fill the rounded corners at the top of the fill
        if (fillHeight > height - radius * 2) {
            int cornerHeight = fillHeight - (height - radius * 2);
            for (int i = 0; i < radius && i < cornerHeight; i++) {
                display.drawFastHLine(x + radius - i, y + height - fillHeight + i, i * 2, GxEPD_BLACK);
            }
        }
    }
}

void drawStarCluster(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                    int16_t x, int16_t y,
                    int16_t width,
                    int16_t height) {
    // Clear area before drawing
    display.fillRect(x, y, width, height, GxEPD_WHITE);
    
    // Calculate grid spacing based on area size
    int gridX = width / 4;  // 4 columns
    int gridY = height / 4; // 4 rows
    
    // Draw stars in a grid pattern with slight randomization
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            // Add slight randomization to star positions
            int offsetX = random(-gridX/4, gridX/4);
            int offsetY = random(-gridY/4, gridY/4);
            
            int starX = x + (col * gridX) + (gridX/2) + offsetX;
            int starY = y + (row * gridY) + (gridY/2) + offsetY;
            
            // Draw a small cross for each star
            display.drawPixel(starX, starY, GxEPD_BLACK);
            display.drawPixel(starX+1, starY, GxEPD_BLACK);
            display.drawPixel(starX-1, starY, GxEPD_BLACK);
            display.drawPixel(starX, starY+1, GxEPD_BLACK);
            display.drawPixel(starX, starY-1, GxEPD_BLACK);
        }
    }
}

void drawAnimationFrame(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                       int16_t x, int16_t y,
                       const AnimationFrame& frame) {
    // Clear the animation area before drawing the new frame
    display.fillRect(x, y, frame.width, frame.height, GxEPD_WHITE);
    
    // Draw the frame
    display.drawBitmap(x, y, &frame.data[2], frame.width, frame.height, GxEPD_BLACK);
}

void drawIcon(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
             int16_t x, int16_t y,
             int16_t width, int16_t height,
             IconType icon) {
    // Clear the area
    display.fillRect(x, y, width, height, GxEPD_WHITE);
    
    switch (icon) {
        case IconType::BATTERY: {
            // Draw battery outline
            int terminalWidth = width / 4;
            int terminalHeight = height / 3;
            display.drawRect(x, y, width - terminalWidth, height, GxEPD_BLACK);
            display.fillRect(x + width - terminalWidth, y + terminalHeight, 
                           terminalWidth, height - (2 * terminalHeight), GxEPD_BLACK);
            break;
        }
        case IconType::WATER_DROP: {
            // Draw a water droplet shape
            int centerX = x + width / 2;
            int topY = y;
            int bottomY = y + height - 1;
            int maxWidth = width * 3 / 4;  // Maximum width at bottom
            
            // Draw the droplet using scanline fill
            for (int yscan = topY; yscan <= bottomY; yscan++) {
                // Calculate width at this height using a modified curve for rounder bottom
                float t = (float)(yscan - topY) / (float)(bottomY - topY);
                // Use a combination of parabolic and circular curves for rounder bottom
                float curve = t < 0.5f ? (1 - (1-t)*(1-t)) : sqrt(1 - (t-0.5f)*(t-0.5f)/0.25f);
                int widthAtY = (int)(maxWidth * curve);
                
                // Draw horizontal line at this height
                display.drawFastHLine(centerX - widthAtY/2, yscan, widthAtY, GxEPD_BLACK);
            }
            break;
        }
        case IconType::SUN: {
            // Draw sun with longer rays
            int centerX = x + width/2;
            int centerY = y + height/2;
            int radius = min(width, height) / 3;
            // Draw center circle
            for (int i = -radius; i <= radius; i++) {
                for (int j = -radius; j <= radius; j++) {
                    if (i*i + j*j <= radius*radius) {
                        display.drawPixel(centerX + i, centerY + j, GxEPD_BLACK);
                    }
                }
            }
            // Draw longer rays
            float rayLength = radius * 2.0; // Increased from 1.5 to 2.2
            for (int i = 0; i < 8; i++) {
                float angle = i * M_PI / 4;
                int rayX = centerX + cos(angle) * rayLength;
                int rayY = centerY + sin(angle) * rayLength;
                display.drawLine(centerX, centerY, rayX, rayY, GxEPD_BLACK);
            }
            break;
        }
        case IconType::HEART: {
            // Draw heart shape
            int centerX = x + width/2;
            int centerY = y + height/2;
            int size = min(width, height) / 2;
            for (int i = -size; i <= size; i++) {
                for (int j = -size; j <= size; j++) {
                    float x2 = (float)i/size;
                    float y2 = (float)j/size;
                    if (pow(x2*x2 + y2*y2 - 1, 3) - (x2*x2 * y2*y2*y2) <= 0) {
                        display.drawPixel(centerX + i, centerY + j, GxEPD_BLACK);
                    }
                }
            }
            break;
        }
        case IconType::LIGHTNING: {
            // Draw a simple, bold, vertical lightning bolt (classic style)
            int cx = x + width / 2;
            int top = y + height / 8;
            int bottom = y + height - height / 8;
            int boltWidth = width / 5; // thickness of the bolt
            // Define bolt points (vertical zig-zag)
            int p0x = cx,                p0y = top;                       // Top
            int p1x = cx + boltWidth,    p1y = top + height / 4;          // Down right
            int p2x = cx - boltWidth/2,  p2y = top + height / 2;          // Down left
            int p3x = cx + boltWidth,    p3y = top + 3 * height / 4;      // Down right
            int p4x = cx,                p4y = bottom;                    // Bottom
            // Store points in arrays for scanline fill
            int px[5] = {p0x, p1x, p2x, p3x, p4x};
            int py[5] = {p0y, p1y, p2y, p3y, p4y};
            // Find min and max y
            int minY = top, maxY = bottom;
            // Scanline fill for bold effect
            for (int yscan = minY; yscan <= maxY; yscan++) {
                int nodes = 0;
                int nodeX[5];
                for (int i = 0, j = 4; i < 5; j = i++) {
                    if ((py[i] < yscan && py[j] >= yscan) || (py[j] < yscan && py[i] >= yscan)) {
                        int xint = px[i] + (yscan - py[i]) * (px[j] - px[i]) / (py[j] - py[i]);
                        nodeX[nodes++] = xint;
                    }
                }
                // Sort nodeX
                for (int i = 0; i < nodes - 1; i++) {
                    for (int j = i + 1; j < nodes; j++) {
                        if (nodeX[i] > nodeX[j]) {
                            int temp = nodeX[i]; nodeX[i] = nodeX[j]; nodeX[j] = temp;
                        }
                    }
                }
                // Draw horizontal lines between node pairs for boldness
                for (int i = 0; i < nodes; i += 2) {
                    if (i + 1 < nodes) {
                        display.drawFastHLine(nodeX[i], yscan, nodeX[i + 1] - nodeX[i] + 1, GxEPD_BLACK);
                        // Make it bolder by drawing a second line next to it
                        display.drawFastHLine(nodeX[i], yscan + 1, nodeX[i + 1] - nodeX[i] + 1, GxEPD_BLACK);
                    }
                }
            }
            break;
        }
    }
}

void drawChangeScreenButton(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                          int16_t x, int16_t y,
                          int16_t width, int16_t height,
                          const char* arrow_direction,
                          bool is_selected,
                          uint8_t border_thickness,
                          uint8_t arrow_thickness,
                          uint8_t corner_radius) {
    // Clear the button area
    display.fillRect(x, y, width, height, GxEPD_WHITE);
    
    // Draw rounded rectangle border with specified thickness
    for (uint8_t i = 0; i < border_thickness; i++) {
        // Top and bottom edges
        display.drawFastHLine(x + corner_radius, y + i, width - (corner_radius * 2), GxEPD_BLACK);
        display.drawFastHLine(x + corner_radius, y + height - 1 - i, width - (corner_radius * 2), GxEPD_BLACK);
        
        // Left and right edges
        display.drawFastVLine(x + i, y + corner_radius, height - (corner_radius * 2), GxEPD_BLACK);
        display.drawFastVLine(x + width - 1 - i, y + corner_radius, height - (corner_radius * 2), GxEPD_BLACK);
        
        // Corners
        for (int j = 0; j < corner_radius; j++) {
            // Top-left corner
            if ((j + i) * (j + i) + (corner_radius - j) * (corner_radius - j) <= corner_radius * corner_radius) {
                display.drawPixel(x + corner_radius - j, y + corner_radius - i, GxEPD_BLACK);
            }
            // Top-right corner
            if ((j + i) * (j + i) + (corner_radius - j) * (corner_radius - j) <= corner_radius * corner_radius) {
                display.drawPixel(x + width - corner_radius + j, y + corner_radius - i, GxEPD_BLACK);
            }
            // Bottom-left corner
            if ((j + i) * (j + i) + (corner_radius - j) * (corner_radius - j) <= corner_radius * corner_radius) {
                display.drawPixel(x + corner_radius - j, y + height - corner_radius + i, GxEPD_BLACK);
            }
            // Bottom-right corner
            if ((j + i) * (j + i) + (corner_radius - j) * (corner_radius - j) <= corner_radius * corner_radius) {
                display.drawPixel(x + width - corner_radius + j, y + height - corner_radius + i, GxEPD_BLACK);
            }
        }
    }
    
    // Fill button if selected
    if (is_selected) {
        // Fill main rectangle
        display.fillRect(x + border_thickness, y + border_thickness, 
                        width - (border_thickness * 2), height - (border_thickness * 2), 
                        GxEPD_BLACK);
        
        // Fill corners with black
        for (int i = 0; i < corner_radius - border_thickness; i++) {
            for (int j = 0; j < corner_radius - border_thickness; j++) {
                // Top-left corner
                if ((i + border_thickness) * (i + border_thickness) + (j + border_thickness) * (j + border_thickness) <= (corner_radius - border_thickness) * (corner_radius - border_thickness)) {
                    display.drawPixel(x + corner_radius - j, y + corner_radius - i, GxEPD_BLACK);
                }
                // Top-right corner
                if ((i + border_thickness) * (i + border_thickness) + (j + border_thickness) * (j + border_thickness) <= (corner_radius - border_thickness) * (corner_radius - border_thickness)) {
                    display.drawPixel(x + width - corner_radius + j, y + corner_radius - i, GxEPD_BLACK);
                }
                // Bottom-left corner
                if ((i + border_thickness) * (i + border_thickness) + (j + border_thickness) * (j + border_thickness) <= (corner_radius - border_thickness) * (corner_radius - border_thickness)) {
                    display.drawPixel(x + corner_radius - j, y + height - corner_radius + i, GxEPD_BLACK);
                }
                // Bottom-right corner
                if ((i + border_thickness) * (i + border_thickness) + (j + border_thickness) * (j + border_thickness) <= (corner_radius - border_thickness) * (corner_radius - border_thickness)) {
                    display.drawPixel(x + width - corner_radius + j, y + height - corner_radius + i, GxEPD_BLACK);
                }
            }
        }
    }
    
    // Calculate arrow dimensions
    int arrowSize = min(width, height) / 2;  // Size of the arrow
    int centerX = x + width / 2;
    int centerY = y + height / 2;
    
    // Draw arrow based on direction
    if (strcmp(arrow_direction, "left") == 0) {
        // Draw left arrow with thickness
        for (uint8_t i = 0; i < arrow_thickness; i++) {
            display.drawLine(centerX + arrowSize/2, centerY - i, centerX - arrowSize/2, centerY - i, is_selected ? GxEPD_WHITE : GxEPD_BLACK);
            display.drawLine(centerX - arrowSize/2, centerY - i, centerX, centerY - arrowSize/2 - i, is_selected ? GxEPD_WHITE : GxEPD_BLACK);
            display.drawLine(centerX - arrowSize/2, centerY - i, centerX, centerY + arrowSize/2 - i, is_selected ? GxEPD_WHITE : GxEPD_BLACK);
        }
    } else if (strcmp(arrow_direction, "right") == 0) {
        // Draw right arrow with thickness
        for (uint8_t i = 0; i < arrow_thickness; i++) {
            display.drawLine(centerX - arrowSize/2, centerY - i, centerX + arrowSize/2, centerY - i, is_selected ? GxEPD_WHITE : GxEPD_BLACK);
            display.drawLine(centerX + arrowSize/2, centerY - i, centerX, centerY - arrowSize/2 - i, is_selected ? GxEPD_WHITE : GxEPD_BLACK);
            display.drawLine(centerX + arrowSize/2, centerY - i, centerX, centerY + arrowSize/2 - i, is_selected ? GxEPD_WHITE : GxEPD_BLACK);
        }
    } else if (strcmp(arrow_direction, "up") == 0) {
        // Draw up arrow with thickness
        for (uint8_t i = 0; i < arrow_thickness; i++) {
            // Draw the vertical line (made taller by adjusting the end point)
            display.drawLine(centerX - i, centerY + arrowSize/2, centerX - i, centerY - arrowSize/2, is_selected ? GxEPD_WHITE : GxEPD_BLACK);
            // Draw the left diagonal of the arrow head
            display.drawLine(centerX - i, centerY - arrowSize/2, centerX - arrowSize/4 - i, centerY - arrowSize/4, is_selected ? GxEPD_WHITE : GxEPD_BLACK);
            // Draw the right diagonal of the arrow head
            display.drawLine(centerX - i, centerY - arrowSize/2, centerX + arrowSize/4 - i, centerY - arrowSize/4, is_selected ? GxEPD_WHITE : GxEPD_BLACK);
        }
    }
} 

