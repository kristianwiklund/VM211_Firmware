

/******************************************************************************
  EarthListener by Pieter Sijmons @ Velleman NV
  Based on examples by Adafruit, SparkFun and Tom Igoe
  See tab 'info_and_refs' for more documentation
******************************************************************************/


/***************************************/
/* ---------- DECLARATIONS ----------- */
/***************************************/
/* --- Global Libraries --- */
#include <Wire.h>                               // Wire library
#include <SPI.h>                                // SPI library
#include <EEPROM.h>                             // Library to read & store info in EEPROM long term memory
#include "ccs811.h"

/* --- Local Libraries --- */
#include "src/Adafruit-GFX-Library/Adafruit_GFX.h"     	  // Core graphics library by Adafruit
#include "src/Adafruit-GFX-Library/Fonts/FreeSans9pt7b.h" // Font FreeSans 9pts (as an alternative for basic font)
#include "src/MCUFRIEND_kbv/MCUFRIEND_kbv.h"           	  // TFT library by David Prentice
#include "src/SDmega/SDmega.h"                          	// SD library (mod by PSI to work with MEGA & TFT SD card shield)
#include "src/SparkFun_AS3935/src/SparkFun_AS3935.h"          // AS3935 library by SparkFun
#include "src/TFTLCD-Library/Adafruit_TFTLCD.h"        	  // Hardware-specific library for TFT screen by Adafruit
#include "src/TouchScreen/TouchScreen.h"               	  // TouchScreen library by Adafruit
#include "bitmaps.h"                            			    // Icon library (local, hence the "")
//#include "src/RTClib/RTClib.h"                            // Adafruit RTC library 
#include "config.h"                                       // instead of the magic numbers in the original code
#include <Adafruit_CCS811.h>
#include <Adafruit_BME280.h>
#include <TimeLib.h>
#include <TimeAlarms.h>

#ifdef WITH_ESP01

#include <SPI.h>
#include <WiFiEspAT.h>
#include <PubSubClient.h>
WiFiClient wificlient;

PubSubClient client(wificlient);

bool wifienabled=false;
#endif


#include "screens.h"


BootScreen bootscreen;
InfoScreen infoscreen;
CO2Screen co2screen;
TVOCScreen tvocscreen;
TempScreen tempscreen;
PressureScreen pressurescreen;
HumidityScreen humidityscreen;
ClockScreen clockscreen;
LightningScreen lightningscreen;

Screen *tehScreens[] = { // spelling intentional
		       &co2screen,
		       &tvocscreen,
		       &tempscreen,
		       &pressurescreen,
		       &humidityscreen,
		       &lightningscreen,
		       &clockscreen
};


/* --- first boot --- */
int firstBoot_EEPROMaddr = 5;   //EEPROM long term memory adress that we use to check if the EarthListener has been booted before (default will be true).


// CCS811

uint16_t CO2=0;
uint16_t TVOC=0;
Adafruit_CCS811 myCCS811;

/* --- BME280 air sensor --- */
#define BME280_ADDR 0x77      //Default I2C Address of BME280 sensor, can be 0x76 if jumper closed
Adafruit_BME280 myBME280;

float HUMIDITY_BME280;
float AMBIENTPRESSURE_BME280;
float AMBIENTPRESSURE_BME280_c; //converted to mBar (= Pa / 100);
float ALTITUDE_BME280;
float TEMP_BME280;
float seaLevelPressure = SEALEVELPRESSURE; //pressure at sea level (Pa), needs to be set daily for proper altitude value!
float TEMP_comp = -3.3;   //the EarthListener is warmed up from the TFT screen hence reporting a higher temperature. This compensation corrects this.
float HUMI_comp = 17;     //same for the humidity. Values are experimental and should be changed according to your findings.


/* --- AS3935 sensor --- */
//selection of IIC or SPI interface (also change the jumper JSI on the board!)
boolean AS3935_SPI = true;  //SPI= true & jumper JSI connected - IIC= false & jumper JSI disconnected
int AS3935_SPI_EEPROMaddr = 3;       // address to store this value in long term memory (EEPROM)

// Interrupt flag variable for AS3935 irqs that indicates interrupt has been triggered
// Variables that get changed in interrupt routines need to be declared volatile
// otherwise compiler can optimize them away, assuming they never get changed
volatile int AS3935IrqTriggered;
unsigned long AS3935IrqTriggeredTime = 0;     //time when last detection was triggered;
uint8_t int_src;                              //interrupt source:  This variable holds the number representing the lightning or non-lightning event issued by the lightning detector. 
int minutesSinceAS3935Trigger;                //time since last detection
String lastErrorLine1 =  "No Disturber (yet)...";   //Keep last error message (but set for when there is none)
String lastErrorLine2 =  "Keep Calm and Carry On";  //Keep last error message (but set for when there is none)

//defines
#define AS3935_ADD           0x03  // Default I2C Address of AS3935 sensor: x03 <-- you can use i2C scanner to see the i2c address
#define AS3935_IRQPIN        18    // interrupt pin on board (18 or 19 on MEGA, 2 or 3 on UNO)
#define spiCS                41    // chip select pin
#define INDOOR               0x12  // value for indoor use (do not change)   
#define OUTDOOR              0xE   // value for outdoor use (do not change)
#define LIGHTNING_INT        0x08  // value for lightning source (do not change)  
#define DISTURBER_INT        0x04  // value for disturber source (do not change)
#define NOISE_INT            0x01  // value for noise source (do not change)

boolean AS3935_OUTDOORS;            // Set to 1 to enabled for Outdoor Used, use 0 for Indoor Use
int AS3935_outdoor_EEPROMaddr = 0;          // address to store this value in long term memory (EEPROM)

// Values for modifying the AS3935's settings. All of these values are set in via the globalSense variable and controlled trough the setup page
byte noiseFloor;        // noise threshold value between 1-7, one being the lowest. Default setting is 2
byte watchDogVal;       // Watchdog threshold value between 1-10, one being the lowest. Default setting is 2
byte spike;             // Spike Rejection value between 1-11, one being the lowest. Default setting is 2. 
byte lightningThresh;   // lightning threshold value, can be 1, 5, 9 and 16. Default setting is 1.
boolean maskDisturbers; // Mask disturbers. Values can be true or false. Default setting is true.
byte globalSense;       // global sensitivity (sets values above + maskDisturber). Values are 1=low, 2=medium, 3=high. Default setting is 2.
int globalSense_EEPROMaddr = 4;       // address to store this value in long term memory (EEPROM)

SparkFun_AS3935 lightningSPI;              // define Lightning sensor for SPI
SparkFun_AS3935 lightningIIC(AS3935_ADD);  // define Lightning sensor on specific address for IIC
boolean AS3935_bootOK;                     // value to store OK boot status of the sensor

// touch screen settings
int Xpos; int Ypos; //global positions to store touchscreen location
unsigned long touchedTime = 0;     //time when last touchscreen interaction occurred;
int touchTimeout = 300;           // timeout between touch inputs (ms)

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 320 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 320);



//now declare tft item
MCUFRIEND_kbv tft;    //we use a DRIVER IC ILI9341
 
// speaker variables
boolean BuzzerEnabled;      //1= Buzzer on, 0= Buzzer off => can be modified via TFT interface
int Buzzer_EEPROMaddr = 1;  // address to store this value in long term memory (EEPROM)


// led variables
boolean LEDenabled = 1;     //1= LED on, 0= LED off. Will also declare pins for output if turned on.


/* --- datalogging & timing --- */
// Modifications:
// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work. On our TFT SD card shield we use 10

boolean SDpresent = 0;        //boolean to store if SD card is present during setup
boolean logFileExists = 0;    //boolean to store if logfile exists on SD card

/* --- real time clock - currently set to DS1307 rtc but can be changed to anything that the RTC library supports --- */
//RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // from RTCLib example

unsigned long allSeconds;
int runDays;
unsigned long secsRemaining;
int runHours;
int runMinutes;
int runSeconds;
int lastSecond = 99;        //we set this value so we can know when we just booted
int secondCounter = 0;      //counter to calculate the number of seconds that have been passed between reads
int readCounter = 0; // count how many sensor reads have been done between loggings
int loggingInterval = LOGGING_INTERVAL;   //interval to log the values to the SD card. Default: 10, min: 1, maximum: 3600 (1 hour)


/* --- menus --- */
int currentScreenNr = 0;        //start with bootscreen
int previousScreenNr;
boolean slideShowPlaying;        //is the slide show playing? yes=1, no=0
int slideshowTimer = 5;         //time (in seconds) to show each slide
unsigned long timeStartSlide = 0;     //time when slide was first shown;


/* --- metric / imperial switch --- */
boolean MetricON;  //boolean to check if values of temperature and lightning distance are set in Celsius/km or Fahrenheit/miles => can be modified via TFT interface
int MetricON_EEPROMaddr = 2;  // address to store this value in long term memory (EEPROM)



/***************************************/
/* ----------- LOOP CODE ------------ */
/***************************************/
void loop(void)
{
  String dataString = "";
#ifdef WITH_ESP01
  // mqtt loop
  client.loop();
#endif

  // update the alarm handling
  Alarm.delay(0);
  
  //timing: get time since boot: will write to global vars 
  getTimeSinceBoot();

  //get touch input (but only after XXX ms after last touchscreen event!)
  if((millis() - touchedTime) > touchTimeout)
  {
    if(touchedMe())
    {
      touchedTime = millis();
      //Serial.print("We have been touched! X=");Serial.print(Xpos);Serial.print(", Y="); Serial.println(Ypos);
      inputControl();
      showScreen(currentScreenNr);  //0:Boot, 1:Info screen, 2:Setup, 3:eCO², 4:Temperature 5:Pressure, 6:TVOC, 7:Humidity, 8:Lightning, 81:Lightning after interrupt
    }
  }

  //  Serial.println(secondCounter);
  // only update measurements every UPDATE_INTERVAL if we are past "logginginterval"
  if (secondCounter > UPDATE_INTERVAL) {
    //    Serial.println("Updating sensors");

#ifdef WITH_ESP01
    if (wifienabled && !client.connected()) {
      mqtt_reconnect();
    }
#endif

    // refactor this
    // (1) read only the sensors that actually exist
    // (2) send only the sensors that actually exist to mqtt
    // (3) log only the sensors that actually exist to the sdcard

    // which sensors exist?
    // the ones with active screen...
 
    
    /****** poll sensors & update vars + log to SD *****/
    // make a string for assembling the data to log on the SD card & add the current time:
    
    // char buf[17];
    // sprintf(buf,"%02d %02d:%02d:%02d",runDays,runHours,runMinutes,runSeconds);
    // dataString = buf;
    // dataString += ",";
    
    //calculate minutes between current time & time when AS3935sensor was triggered
    if(lightningscreen.isEnabled())
      minutesSinceAS3935Trigger = returnMinutes(millis() - AS3935IrqTriggeredTime);

    //read data from BME280
    if(humidityscreen.isEnabled()) {
      HUMIDITY_BME280 = myBME280.readHumidity();
      if(HUMIDITY_BME280 <= (100 - HUMI_comp))  //make sure we don't have values > 100%
	{
	  HUMIDITY_BME280 = HUMIDITY_BME280 + HUMI_comp;
	}
    }

    if(pressurescreen.isEnabled()) {
      AMBIENTPRESSURE_BME280 = myBME280.readPressure();
      AMBIENTPRESSURE_BME280_c = AMBIENTPRESSURE_BME280 / 100; //convert Pa to mBar
      ALTITUDE_BME280 = myBME280.readAltitude(seaLevelPressure);
    }

    if(tempscreen.isEnabled()) {
      TEMP_BME280 = myBME280.readTemperature();
      //compensate temp & humi data
      TEMP_BME280 = TEMP_BME280 + TEMP_comp;
    }
    
    // //add data to dataString to write to SD
    // dataString += TEMP_BME280,2;
    // dataString += ",";
    // dataString += HUMIDITY_BME280,2;
    // dataString += ",";
    // dataString += AMBIENTPRESSURE_BME280_c,2;
    // dataString += ",";
    // dataString += ALTITUDE_BME280,2;
    // dataString += ",";
    
    // checks for sensor availability is done inside the ccs811 code
    ccs811_loop();
    //add data to dataString to write to SD
    // assumes that we have data btw.
    dataString += CO2;
    dataString += ",";
    dataString += TVOC;
    
    secondCounter = 0;
    readCounter++;
  }
  if (readCounter > LOGGING_INTERVAL) {
    String x="";

   #ifdef WITH_ESP01
    // to mqtt
    if(wifienabled && client.connected()) {
      if(humidityscreen.isEnabled()) 
	f2mqtt("vm211/humidity",HUMIDITY_BME280);
      if(pressurescreen.isEnabled())  {
	f2mqtt("vm211/pressure",AMBIENTPRESSURE_BME280_c);
	f2mqtt("vm211/altitude",ALTITUDE_BME280);
      }

      if(tempscreen.isEnabled())
	f2mqtt("vm211/temperature",TEMP_BME280);

      if(co2screen.isEnabled())
	f2mqtt("vm211/co2", CO2);
      if(tvocscreen.isEnabled())
	f2mqtt("vm211/tvoc", TVOC);
      

    }
    #endif
    readCounter = 0;

  } 

  //only do next code every second
  if(runSeconds != lastSecond)
    {
      secondCounter++;
      /****show screens****/
      //interrupt from lightning sensor! -> show lightning screen, otherwise show info screen
      //we only do this if AS3935_bootOK is true (otherwise noise from sensor)
      if (AS3935IrqTriggered && AS3935_bootOK)
        {
          Serial.println("Interrupt from lightning sensor");
          
          //check interrupt source
          Alarm.delay(5); //wait so not to overflow the bus
          if(AS3935_SPI)
	    {
	      int_src = lightningSPI.readInterruptReg();
	    }else
	    {
	      int_src = lightningIIC.readInterruptReg();
	    }
          
          showScreen(81);   //0:Boot, 1:Info screen, 2:Setup, 3:eCO², 4:Temperature 5:Pressure, 6:TVOC, 7:Humidity, 8:Lightning, 81:Lightning after interrupt
          AS3935IrqTriggered = 0;
        }
      else
        {
          //check if slideshow is on
          if(slideShowPlaying)
	    { 
	      //automatically change the screen every time the slideshowTimer value has been reached
	      if( ( millis() - timeStartSlide ) > (slideshowTimer * 1000) )
		{
                  previousScreenNr = currentScreenNr;
		  //		  Serial.print("Changing screen ");
		  //		  Serial.print(currentScreenNr);
		  currentScreenNr = getnewcurrentscreen(currentScreenNr);
		  
		}
	    }
          
          //if a screen has been changed, reset the slide timer
          if(currentScreenNr != previousScreenNr)
	    {
	      timeStartSlide = millis();
	    }
          showScreen(currentScreenNr);  //0:Boot, 1:Info screen, 2:Setup, 3:eCO², 4:Temperature 5:Pressure, 6:TVOC, 7:Humidity, 8:Lightning, 81:Lightning after interrupt
        }
      
      
        //update lastSecond so this loop can only be run at the next second
      lastSecond = runSeconds;
    }
}
