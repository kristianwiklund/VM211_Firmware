// configuration file to hold some basic constants to avoid magic numbers - refactoring work in progress

#define SWversion "v4.0.5.kw"

#ifndef CONFIG_H
#define CONFIG_H

// clock settings
#define NTP_HOST "time.windows.com"

#define CLOCK_DST_RULE {"EDT", Last, Sun, Mar, 3, 120} // UTC + 2 hours in summer
#define CLOCK_STD_RULE {"EST", Last, Sun, Oct, 2, 60} // UTC + 1 hour in winter


// default interval to log to SDCARD and mqtt
#define UPDATE_INTERVAL 10 // update sensors every 10 seconds
#define LOGGING_INTERVAL 30 // how many readings between logs? 10x30=300 = 5 minutes by default

#define WITH_ESP01 // for wifi
#define MQTT_HOST "10.168.0.194"


#define INITIAL_SLIDESHOW_STATUS 1 // change to 0 to not run slideshow from start. Using the slideshow from start will show the config screens, before getting into the loop correctly.


// for BME280

#define SEALEVELPRESSURE (1013.25)

// display positioning
#define startXimg 10
#define startYimg 175
#define widthImg 300
#define heightImg 1

// clock settings

#define CLOCK_X 160 // center x
#define CLOCK_Y 130 // center y
#define CLOCK_R 60  // radius
#define CLOCK_CR 2  // centre blob radius
#define CLOCK_H_LEN 4 // length of hour marks
#define CLOCK_H_HAND 0.7 // length of hour hand (as fraction of the clock radius)


// - from original vmm_...ino file, should normally not be touched!

#define CCS811_ADDR 0x5B //Default I2C Address


/* --- LCD panel with touch --- */
//be aware we use the TFT LCD on a Arduino Mega
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
#define touchPin 38 //pin to use for reading touchscreen

#define TS_MINX 100
#define TS_MAXX 920
#define TS_MINY 70
#define TS_MAXY 900
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3         // Chip Select goes to Analog 3
#define LCD_CD A2         // Command/Data goes to Analog 2
#define LCD_WR A1         // LCD Write goes to Analog 1
#define LCD_RD A0         // LCD Read goes to Analog 0
#define LCD_RESET A4      // Normally A4, but can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
// http://www.barth-dev.de/online/rgb565-color-picker/
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0xD69A
#define GREYY   0xAD55

/* --- Speaker --- */
//pin setup for speaker
#define BuzzerPin A10

/* --- RGB LED --- */
#define LEDgnd A11
#define LEDg A12
#define LEDr A13
#define LEDb A14

#endif
