#pragma once

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

// Display dimensions
#define EPD_WIDTH       200
#define EPD_HEIGHT      200

// Page definitions
enum Page {
    HOME_PAGE,
    DRINK_WATER_PAGE,
    SUNBATHE_PAGE,
    PET_POMMY_PAGE,
    STORE_PAGE
};

// Function declarations
void drawHomePage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display);
void drawDrinkWaterPage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display);
void drawSunbathingPage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display);
void drawPetPommyPage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display);
void drawStorePage(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>& display); 