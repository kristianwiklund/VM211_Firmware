#include <Arduino.h>
#include <stdint.h>
#include "screens.h"
#include "config.h"
#include "functions.h"
#include "bitmaps.h" // yes - this is included twice for now, once here and once in the main. Remove once all code is here!


// graphics includes
#include "src/TFTLCD-Library/Adafruit_TFTLCD.h"        	  // Hardware-specific library for TFT screen by Adafruit
#include "src/Adafruit-GFX-Library/Fonts/FreeSans9pt7b.h"
#include "src/MCUFRIEND_kbv/MCUFRIEND_kbv.h"           	  // TFT library by David Prentice
extern MCUFRIEND_kbv tft; 

// ----- 8< --- Base class --- 8< -----

bool Screen::isEnabled() {
  return isenabled;
}

void Screen::setEnabled(bool x) {
  isenabled = x;
}

// draw the screen contents
bool Screen::draw() {
  // do something
  // return false if we are not supposed to draw this screen
  return isenabled;
};

// ----- 8< --- Bootscreen --- 8< -----

BootScreen::BootScreen() {
  setEnabled(true); // boot screen is always visible
}

bool BootScreen::draw() {

  if(!Screen::draw())
    return;
  
    //print logo Velleman
    controlLogo(GREYY);
    //print logo EarthListener
    showbgd(35, 65, EarthListener_200x150, 200, 150, WHITE, BLACK);
    //print SW version
    tft.setFont(&FreeSans9pt7b);
    tft.setCursor(235, 207);
    tft.setTextColor(GREYY);  
    tft.setTextSize(1);
    tft.print(SWversion);
    //set font to standard system font
    tft.setFont();
    //test LED
    controlLED('W');
    delay(1500);
    //cycle logo & LED color
    showbgd(35, 65, EarthListener_200x150, 200, 150, RED, BLACK);
    controlLED('R');
    delay(500);
    showbgd(35, 65, EarthListener_200x150, 200, 150, GREEN, BLACK);
    controlLED('G');
    delay(500);
    showbgd(35, 65, EarthListener_200x150, 200, 150, BLUE, BLACK);
    controlLED('B');
    delay(500);
    //clear logo & version for sensor info
    tft.fillRect(0,65,320,175,BLACK);
    controlLED('0');

    // yes, draw this one
    return (true);
  }

// ----- 8< --- Infoscreen --- 8< -----

InfoScreen::InfoScreen() {
  setEnabled(true); // boot screen is always visible
}

bool InfoScreen::draw() {

  if(!Screen::draw())
    return;
  
  //print icons
  showbgd(40, 72, eCO2_65x50, 65, 50, WHITE, BLACK);
  showbgd(127, 72, temperature_65x50, 65, 50, WHITE, BLACK);
  showbgd(214, 72, pressure_65x50, 65, 50, WHITE, BLACK);
  showbgd(40, 148, tvoc_65x50, 65, 50, WHITE, BLACK);
  showbgd(127, 148, humidity_65x50, 65, 50, WHITE, BLACK);
  showbgd(214, 148, lightning_65x50, 65, 50, WHITE, BLACK);
  //print values from sensors
  printValues();

  return (true);
}
