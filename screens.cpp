#include <Arduino.h>
#include <stdint.h>
#include "screens.h"
#include "config.h"
#include "functions.h"
#include "bitmaps.h"

// bitmap prototypes


// graphics includes
#include "src/TFTLCD-Library/Adafruit_TFTLCD.h"        	  // Hardware-specific library for TFT screen by Adafruit
#include "src/Adafruit-GFX-Library/Fonts/FreeSans9pt7b.h"
#include "src/MCUFRIEND_kbv/MCUFRIEND_kbv.h"           	  // TFT library by David Prentice
extern MCUFRIEND_kbv tft; 

// ----- 8< --- code --- 8< -----

bool Screen::isEnabled() {
  return isenabled;
}

// draw the screen contents
void Screen::draw() {
};

// ----- 8< --- code --- 8< -----

void BootScreen::draw() {

    Screen::draw();
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

  }

