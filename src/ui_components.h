#pragma once

#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include "ui_config.h"
#include "animation.h"

// Icon types for status indicators
enum class IconType {
    BATTERY,
    WATER_DROP,
    SUN,
    HEART,
    LIGHTNING
};

// Function declarations
void drawRectangle(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, 
                  int16_t x, int16_t y, 
                  int16_t width, int16_t height,
                  uint8_t thickness,
                  bool fill);

void drawTitle(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, 
              int16_t x, int16_t y, 
              const char* text);

void drawButton(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display, 
               int16_t x, int16_t y, 
               const char* text);

void drawStatusBar(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                  int16_t x, int16_t y,
                  int16_t width, int16_t height,
                  uint8_t fill,
                  uint8_t radius = UIConfig::CORNER_RADIUS);

void drawStarCluster(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                    int16_t x, int16_t y,
                    int16_t width,
                    int16_t height);

void drawAnimationFrame(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                       int16_t x, int16_t y,
                       const AnimationFrame& frame);

void drawIcon(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
             int16_t x, int16_t y,
             int16_t width, int16_t height,
             IconType icon);

void drawChangeScreenButton(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display,
                          int16_t x, int16_t y,
                          int16_t width, int16_t height,
                          const char* arrow_direction,
                          bool is_selected,
                          uint8_t border_thickness,
                          uint8_t arrow_thickness,
                          uint8_t corner_radius = UIConfig::CORNER_RADIUS); 