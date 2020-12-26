// draw a clock on the screen - requires that a real time clock chip is present. 
//
// (C) 2020 Kristian Wiklund
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//
#include <Arduino.h>
#include "config.h"
#include <math.h>
#include "screens.h"
#include <TimeLib.h>
#include "clock.h"

// clock includes
//#include "src/RTClib/RTClib.h"                            // Adafruit RTC library
//extern RTC_DS1307 rtc;

#include <DS1307RTC.h>  

// graphics includes
#include "src/TFTLCD-Library/Adafruit_TFTLCD.h"        	  // Hardware-specific library for TFT screen by Adafruit
#include "src/Adafruit-GFX-Library/Fonts/FreeSans9pt7b.h"
#include "src/MCUFRIEND_kbv/MCUFRIEND_kbv.h"           	  // TFT library by David Prentice
extern MCUFRIEND_kbv tft;

// --- begin code from timelib example ---


void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}


// --- end code from timelib example ---


// return TRUE if we are able to sync to at least one clock source
bool setup_clock() {

  time_t ntptime=0;
  
#ifdef WITH_ESP01
  Serial.println("Clock: Getting NTP time"); 
  ntptime = getNtpTime();
#endif

  // code partially from ds1307 lib example

  tmElements_t tm;


  // which source to use?
  // go for NTP, if available. Otherwise, use the RTC

  if(ntptime) {
    setSyncProvider(getNtpTime);
    if (timeStatus() != timeSet) { 
      Serial.println("Clock: Unable to sync with NTP - trying RTC instead");
    }
    else {
      Serial.println("Clock: NTP has set the system time");
      digitalClockDisplay();
      RTC.read(tm);
      if(RTC.chipPresent()) {
	Serial.println("Clock: Updating RTC chip time to NTP time");
	RTC.set(ntptime);
	Serial.print("Clock: RTC seconds = ");
	Serial.print(RTC.get());
      } else {
	Serial.println("Clock: RTC fail error");
      }
      return true;
    }

  }

  // if ntp failed, go for the ds1307

  Serial.println("Clock: No NTP available, using DS1307 for time base");
  RTC.read(tm);
  if(!ntptime) {
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet) {
      Serial.println("Clock: Unable to sync with the RTC - unable to keep real time!");

      if (RTC.read(tm)) {
	Serial.println("Clock: RTC read okay");
	digitalClockDisplay();
      } else {
	if (RTC.chipPresent()) {
	  Serial.println("Clock: The DS1307 is stopped.  Please run the TimeLib SetTime");
	  Serial.println("Clock example to initialize the time and begin running.");
	  Serial.println();
	} else {
	  Serial.println("Clock: DS1307 read error!  Please check the circuitry.");
	  Serial.println();
	}
      }
    }
    else {
      Serial.println("Clock: RTC has set the system time");
      return true;
    }
  }

  return false;
}
  


ClockScreen::ClockScreen() {

}

bool ClockScreen::draw() {
  static int oldsec=0, oldmin=0, oldhour=0;
  if(!isEnabled())
    return false;
  
  // this clears the screen in the caller
  // tft.fillRect(10,60,310,180,BLACK); 
  // which means that 10,60,10+310,60+180 is the area where we can play

  // draw a large circle with a small circle inside
  tft.drawCircle(CLOCK_X,CLOCK_Y,CLOCK_R, WHITE);
  tft.fillCircle(CLOCK_X, CLOCK_Y, CLOCK_CR, WHITE);

  // draw hour marks  
  // replace the trig calls with precalculated values for more efficiency, if needed

  for(int i=0;i<12;i++) {
    int x1,y1,x2,y2;
    
    x1 = sin(i*2*PI/12)*CLOCK_R+CLOCK_X;
    x2 = sin(i*2*PI/12)*(CLOCK_R-CLOCK_H_LEN)+CLOCK_X;
    y1 = cos(i*2*PI/12)*CLOCK_R+CLOCK_Y;
    y2 = cos(i*2*PI/12)*(CLOCK_R-CLOCK_H_LEN)+CLOCK_Y;

    tft.drawLine(x1,y1,x2,y2,WHITE);
  }
  

// cleanup

  if(oldsec!=second()) {
    tft.drawLine(CLOCK_X, CLOCK_Y, CLOCK_R*sin(oldsec*2*PI/60)+CLOCK_X,-CLOCK_R*cos(oldsec*2*PI/60)+CLOCK_Y, BLACK);
  }

if(oldmin!=minute()) {
    tft.fillTriangle(CLOCK_R*sin(oldmin*2*PI/60)+CLOCK_X,-CLOCK_R*cos(oldmin*2*PI/60)+CLOCK_Y,
       -CLOCK_H_LEN*cos(oldmin*2*PI/60)+CLOCK_X,-CLOCK_H_LEN*sin(oldmin*2*PI/60)+CLOCK_Y,  
       CLOCK_H_LEN*cos(oldmin*2*PI/60)+CLOCK_X,CLOCK_H_LEN*sin(oldmin*2*PI/60)+CLOCK_Y,
      BLACK);
  }


 if(oldhour!=hour() || oldmin!=minute()) {
    tft.fillTriangle(CLOCK_H_HAND*CLOCK_R*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,-CLOCK_H_HAND*CLOCK_R*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,
       -CLOCK_H_LEN*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,-CLOCK_H_LEN*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,  
       CLOCK_H_LEN*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,CLOCK_H_LEN*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,
      BLACK);
  }

// redraw  
  
 

    
  oldmin=minute();
  oldsec=second();
  oldhour=hour();

  tft.fillTriangle(CLOCK_H_HAND*CLOCK_R*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,-CLOCK_H_HAND*CLOCK_R*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,
  -CLOCK_H_LEN*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,-CLOCK_H_LEN*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,  
  CLOCK_H_LEN*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,CLOCK_H_LEN*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,
  GREYY);

  tft.fillTriangle(CLOCK_R*sin(oldmin*2*PI/60)+CLOCK_X,-CLOCK_R*cos(oldmin*2*PI/60)+CLOCK_Y,
  -CLOCK_H_LEN*cos(oldmin*2*PI/60)+CLOCK_X,-CLOCK_H_LEN*sin(oldmin*2*PI/60)+CLOCK_Y,  
  CLOCK_H_LEN*cos(oldmin*2*PI/60)+CLOCK_X,CLOCK_H_LEN*sin(oldmin*2*PI/60)+CLOCK_Y,
   GREY);
   
 tft.drawLine(CLOCK_X, CLOCK_Y, CLOCK_R*sin(second()*2*PI/60)+CLOCK_X,-CLOCK_R*cos(second()*2*PI/60)+CLOCK_Y, WHITE);
  
 return true;
}


