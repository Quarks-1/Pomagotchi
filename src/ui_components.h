#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include "animation.h"

// Icon types available in the system
enum class IconType {
    BATTERY,
    WATER_DROP,
    SUN,
    HEART,
    FOOD,
    LIGHTNING
};

// Draw a rectangle with standard styling
void drawRectangle(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, 
                  int16_t x, int16_t y, 
                  int16_t width, int16_t height,
                  uint8_t thickness = 1,
                  bool fill = false);

// Draw a decorative star cluster
void drawStarCluster(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                    int16_t x, int16_t y,
                    int16_t width,
                    int16_t height);

// Draw an animation frame
void drawAnimationFrame(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                       int16_t x, int16_t y,
                       const AnimationFrame& frame);

// Draw a title with standard styling
void drawTitle(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, int16_t x, int16_t y, const char* text);

// Draw a button with standard styling
void drawButton(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, int16_t x, int16_t y, const char* text);

// Draw a status bar with name, icon, and level
void drawStatusBar(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, 
                  int16_t x, int16_t y, 
                  const char* name, 
                  const uint8_t* icon, 
                  uint8_t level);

// Draw a status bar with fill level
void drawStatusBar(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                  int16_t x, int16_t y,
                  int16_t width, int16_t height,
                  uint8_t fill,
                  uint8_t radius = 2);

// Draw an icon
void drawIcon(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
             int16_t x, int16_t y,
             int16_t width, int16_t height,
             IconType icon);

#endif // UI_COMPONENTS_H 