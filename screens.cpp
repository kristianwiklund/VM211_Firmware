#include <Arduino.h>
#include <stdint.h>
#include "screens.h"
#include "config.h"
#include "functions.h"
#include "bitmaps.h" // yes - this is included twice for now, once here and once in the main. Remove once all code is here!

// external globals


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

// ----- 8< --- CO2 screen --- 8< -----

extern int CO2;

CO2Screen::CO2Screen() {
  // nothing to see
}

bool CO2Screen::draw() {

  //local vars for plotting the indicator onto the scales/graph
  int Xindic;
  float calcVal;
  
  if(!Screen::draw())
    return;
  
  //print value, icon & update LED
  tft.setFont();  //standard system font
  tft.setTextSize(3);
  tft.setCursor(140, 120);
  if (CO2 < 800)
    {
      tft.setTextColor(GREEN,BLACK);
      showbgd(10, 75, eCO2_100x77, 100, 77, GREEN, BLACK);
      controlLED('G');
      controlLogo(GREEN);
    }
  else if ((CO2 >=800) && (CO2 < 1000))
    {
      tft.setTextColor(BLUE,BLACK);
      showbgd(10, 75, eCO2_100x77, 100, 77, BLUE, BLACK);
      controlLED('B');
      controlLogo(BLUE);
    }
  else if ((CO2 >= 1000) && (CO2 < 1500))
    {
      tft.setTextColor(YELLOW,BLACK);
      showbgd(10, 75, eCO2_100x77, 100, 77, YELLOW, BLACK);
      controlLED('Y');
      controlLogo(YELLOW);
    }
  else if (CO2 >= 1500)
    {
      tft.setTextColor(RED,BLACK);
      showbgd(10, 75, eCO2_100x77, 100, 77, RED, BLACK);
      controlLED('R');
      controlLogo(RED);
    }      
  if(CO2 < 10)          {  tft.print("   "); }    //add leading spaces
  else if(CO2 < 100)    {  tft.print("  ");  }
  else if(CO2 < 1000)   {  tft.print(" ");   }
  tft.print(CO2);
  tft.println(" ppm");
  /*
  //print scale with fillRect(startX, startY, width, height, color)
  tft.fillRect( 10, 175, 46, 18, GREEN);
  tft.fillRect( 57, 175, 22, 18, BLUE);
  tft.fillRect( 80, 175, 56, 18, YELLOW);
  tft.fillRect( 137,175,173, 18, RED);
  */
  //print scale from bitmap file - file is 1 line, so print it 18 times
  for( int zz = 0; zz < 18; zz++)
    {
      tft.setAddrWindow(startXimg, startYimg + zz, startXimg + widthImg - 1, startYimg + zz + heightImg - 1);
      tft.pushColors((const uint8_t*)eCO2_graph_300x1, widthImg * heightImg, 1, false);
    }
  tft.fillRect( 10,193,300,  6, BLACK);  //erase the bottom under the scale (from previous indicator)
  //draw indicator with drawLine(startX, startY, endX, endY, color)
  //calcVal = ( ( (CO2-400) / (3000-400) ) * 300 ) + 10;
  calcVal = CO2-400;
  calcVal = calcVal / (3000-400);
  calcVal = calcVal * 300;
  calcVal = calcVal + 10;    
  Xindic = (int) calcVal;
  if(Xindic < 10){Xindic = 10;}
  else if(Xindic > 300){Xindic = 300;}
  tft.drawLine(Xindic, 175, Xindic, 198, WHITE);
  //print values of scale
  tft.setTextSize(1); 
  tft.setCursor(10, 165); tft.setTextColor(GREEN,BLACK); tft.print("400");
  tft.setCursor(48, 165); tft.setTextColor(BLUE,BLACK); tft.print("801");
  tft.setCursor(81, 165); tft.setTextColor(YELLOW,BLACK); tft.print("1001");
  tft.setCursor(138, 165); tft.setTextColor(RED,BLACK); tft.print("1501"); 
  tft.setCursor(285, 165); tft.setTextColor(RED,BLACK); tft.print("3000");       
  
  return (true);
}

// ----- 8< --- TVOC screen --- 8< -----

extern int TVOC;

TVOCScreen::TVOCScreen() {
  // nothing to see
}

bool TVOCScreen::draw() {
  //local vars for plotting the indicator onto the scales/graph
  int Xindic;
  float calcVal;
 
  if(!Screen::draw())
    return;
  
  //print value, icon & update LED
  tft.setFont();  //standard system font
  tft.setTextSize(3);
  tft.setCursor(140, 120);
  if (TVOC <= 50)
    {
      tft.setTextColor(GREEN,BLACK);
      showbgd(10, 75, tvoc_100x77, 100, 77, GREEN, BLACK);
      controlLED('G');
      controlLogo(GREEN);
    }
  else if ((TVOC > 50) && (TVOC <= 150))
    {
      tft.setTextColor(YELLOW,BLACK);
      showbgd(10, 75, tvoc_100x77, 100, 77, YELLOW, BLACK);
      controlLED('Y');
      controlLogo(YELLOW);
    }
  else if (TVOC > 150)
    {
      tft.setTextColor(RED,BLACK);
      showbgd(10, 75, tvoc_100x77, 100, 77, RED, BLACK);
      controlLED('R');
      controlLogo(RED);
    }
  if(TVOC < 10){          tft.print("   ");  }   //add leading spaces
  else if(TVOC < 100){    tft.print("  ");   }
  else if(TVOC < 1000){   tft.print(" ");    }
  tft.print(TVOC);
  tft.println(" ppb");
  /*
  //print scale with fillRect(startX, startY, width, height, color)
  tft.fillRect( 10, 175, 50, 18, GREEN);
  tft.fillRect( 61, 175, 99, 18, YELLOW);
  tft.fillRect( 161,175,149, 18, RED);
  */
  //print scale from bitmap file - file is 1 line, so print it 18 times
  for( int zz = 0; zz < 18; zz++)
    {
      tft.setAddrWindow(startXimg, startYimg + zz, startXimg + widthImg - 1, startYimg + zz + heightImg - 1);
      tft.pushColors((const uint8_t*)tvoc_graph_300x1, widthImg * heightImg, 1, false);
    }
  tft.fillRect( 10,193,300, 6, BLACK);  //erase the bottom under the scale (from previous indicator)
  //draw indicator with drawLine(startX, startY, endX, endY, color)
  //calcVal = TVOC + 10;
  calcVal = TVOC + 10;
  Xindic = (int) calcVal;
  if(Xindic < 10){Xindic = 10;}
  else if(Xindic > 300){Xindic = 300;}
  tft.drawLine(Xindic, 175, Xindic, 198, WHITE);
  //print values of scale
  tft.setTextSize(1); 
  tft.setCursor(10, 165); tft.setTextColor(GREEN,BLACK); tft.print("0");
  tft.setCursor(60, 165); tft.setTextColor(YELLOW,BLACK); tft.print("51");
  tft.setCursor(160, 165); tft.setTextColor(RED,BLACK); tft.print("151");
  tft.setCursor(290, 165); tft.setTextColor(RED,BLACK); tft.print("300");

  return (true);
}
