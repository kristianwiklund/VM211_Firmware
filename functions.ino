#include "config.h"
#include "ccs811.h"

#define NRSCREENS 7
int nrOfScreens=0;
#define MAXSCREEN (NRSCREENS+2)

// calculate the next screen number
int getnewcurrentscreen(int csn) {
  if (csn+1 > MAXSCREEN) {
    csn = 2; // wrapover
  }
  
  if (tehScreens[csn+1-3] && tehScreens[csn+1-3]->isEnabled()) {
    //    Serial.print("->");
    //    Serial.println(csn+1);
    return (csn+1);
  }
  else
    return getnewcurrentscreen(csn+1);  
}


/***************************************/
/* ------------ FUNCTIONS ------------ */
/***************************************/

//print final air quality (set color & LED)
void Print_Level(int data1) {
  
  if (data1 < 600)
  {
    tft.setTextColor(GREEN,BLACK);
    tft.println("Excellent Air");
    controlLED('G');
    controlLogo(GREEN);
  }
  else if ((data1 >= 600) && (data1 < 800))
  {
    tft.setTextColor(GREEN,BLACK);
    tft.println("  Good Air   ");
    controlLED('G');
    controlLogo(GREEN);
  }
  else if ((data1 >= 800) && (data1 < 1000))
  {
    tft.setTextColor(BLUE,BLACK);
    tft.println("  Fair Air   ");
    controlLED('B');
    controlLogo(BLUE);
  }
  else if ((data1 >= 1000) && (data1 < 1500))
  {
    tft.setTextColor(YELLOW,BLACK);
    tft.println("Mediocre Air ");
    controlLED('Y');
    controlLogo(YELLOW);
  }
  else if (data1 >= 1500)
  {
    tft.setTextColor(RED,BLACK);
    tft.println("   Bad Air   ");
    controlLED('R');
    controlLogo(RED);
  }
}





//function to show red, green and blue pattern on screen
//returns time it took to run the function
unsigned long testFillScreen()
{
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}


//update the vars in the CCS811 sensor with temp & humi data from BME280 sensor
void updateCCS811vars(float TEMP, float HUMIDITY)
{
  //Serial.println("Updating CCS811 sensor with environment values...");
  myCCS811.setEnvironmentalData( HUMIDITY, TEMP);
  //Serial.print("New humidity= "); Serial.print(HUMIDITY, 2); Serial.println("%");
  //Serial.print("New temp= "); Serial.print(TEMP, 2); Serial.println("°C");
}
  
//interrupt from AS3935 sensor, set var so loop handles this first
void interruptFunction()
{
  AS3935IrqTriggered = 1;
}


//change the LED color according to status
void controlLED(char COLOR)
{
    if(LEDenabled)
    {
      switch (COLOR) 
      {
        case 'R':     //red
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, HIGH);
          digitalWrite(LEDb, LOW);
          break;
        case 'G':     //green
          digitalWrite(LEDg, HIGH);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, LOW);
          break;
        case 'Y':     //yellow
          digitalWrite(LEDg, HIGH);
          digitalWrite(LEDr, HIGH);
          digitalWrite(LEDb, LOW);
          break;
        case 'B':     //blue
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, HIGH);
          break;
        case 'W':     //white
          digitalWrite(LEDg, HIGH);
          digitalWrite(LEDr, HIGH);
          digitalWrite(LEDb, HIGH);
          break;
        case 'O':     //off
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, LOW);
          break;  
        default:
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, LOW);
      }
    }
}


//change the Velleman logo's color
void controlLogo(uint16_t logocolor)
{
    showbgd(103, 20, Velleman_114x36, 114, 36, logocolor, BLACK);
}


//convert the time since boot (in milliseconds) to readable format
void getTimeSinceBoot()
{
  allSeconds=     millis()      /   1000;
  runDays=        allSeconds    /   86400;
  secsRemaining=  allSeconds    %   86400;
  runHours=       secsRemaining /   3600;
  secsRemaining=  secsRemaining %   3600;
  runMinutes=     secsRemaining /   60;
  runSeconds=     secsRemaining %   60;
}


//convert a time in millis to minutes
int returnMinutes(unsigned long Millis)
{
  unsigned long conSeconds = Millis/1000;
  int conMinutes = conSeconds / 60;
  return conMinutes;
}


//check if the touchscreen has been touched & update Xpos & Ypos global vars
boolean touchedMe()
{
  digitalWrite(touchPin, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(touchPin, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
  {
    // scale from 0->1023 to tft.width
    Xpos = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    Ypos = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    /*
    Serial.print("X = "); Serial.print(Xpos);
    Serial.print("\tY = "); Serial.print(Ypos);
    Serial.print("\tPressure = "); Serial.println(p.z);
    */
    return 1; //we have been touched (yeey!)
  }
  else
  {
    return 0; //we haven't been touched (shame...)
  }
}


//function to print info to tft screen
void showScreen(int screenNr)
{
  currentScreenNr = screenNr; //set currentScreenVal
  
  //local vars for plotting the indicator onto the scales/graph
  int Xindic;
  float calcVal;
  //local vars for plotting the scales/graphs
  // replaced by defined in config.h

  //if a screen has been changed, clear the part below the header & update menu buttons
  if(currentScreenNr != previousScreenNr)
  {
    //tft.fillScreen(BLACK); //clear screen
    //we only have to clear part of the screen  (startX, startY, width, height, color)
    tft.fillRect(10,60,310,180,BLACK); 
    //print menu buttons
    printMenuBtn();
  }

  
  //according to each screen, print some info
  switch (screenNr)
  {
    //Boot screen
  case 0:
    // call the thingumajig instead
    bootscreen.draw();
    break;
    
    //Info screen
  case 1:
    infoscreen.draw();
    break;
    
	//Setup screen - too many globals to get fixed quickly as an object - ignoring
  case 2:
    //global font & color
    tft.setFont();  //standard system font
    tft.setTextSize(1);
    tft.setTextColor(GREYY,BLACK); 
  
    //buzzer
    tft.setCursor(50, 125);
    if(BuzzerEnabled)
      {
	showbgd(40, 72, buzzer_on_65x50, 65, 50, GREYY, BLACK);
	tft.print("Buzzer ON ");
      }
    else
      {
	showbgd(40, 72, buzzer_off_65x50, 65, 50, GREYY, BLACK);
	tft.print("Buzzer OFF");
      }
  
    //metric/imperial
    tft.setCursor(140, 125);
    if(MetricON)
      {
	showbgd(127, 72, metrical_65x50, 65, 50, GREYY, BLACK);
	tft.print("Metrical");
      }
    else
      {
	showbgd(127, 72, imperial_65x50, 65, 50, GREYY, BLACK);
	tft.print("Imperial");
      }
  
    //slideshow timer
    showbgd(214, 72, slideshow_65x50, 65, 50, GREYY, BLACK);
    tft.setCursor(215, 125);
    tft.print("Timer: "); tft.print(slideshowTimer); if(slideshowTimer < 10){tft.print(" ");}
  
    //lightning sensitivity
    tft.setCursor(40, 200);
    if(globalSense == 1)
      {      
	showbgd(40, 148, lightning_low_65x50, 65, 50, GREYY, BLACK);
	tft.print("Sense Low   ");
      }
    else if(globalSense == 2)
      {
	showbgd(40, 148, lightning_65x50, 65, 50, GREYY, BLACK);
	tft.print("Sense Medium");
      }
    else if(globalSense == 3)
      {
	showbgd(40, 148, lightning_high_65x50, 65, 50, GREYY, BLACK);
	tft.print("Sense High  ");
      }
    else
      {
	showbgd(40, 148, lightning_65x50, 65, 50, RED, BLACK);
	tft.print("Sense Error!");
      }
  
    //indoor/outdoor mode
    tft.setCursor(135, 200);
    if(AS3935_OUTDOORS)
      {
	showbgd(127, 148, outdoor_65x50, 65, 50, GREYY, BLACK);
	tft.print("Outdoor");
      }
    else
      {
	showbgd(127, 148, indoor_65x50, 65, 50, GREYY, BLACK);
	tft.print(" Indoor");
      }
  
    //chip interface AS3935
    tft.setCursor(215, 200);
    if(AS3935_SPI)
      {
	showbgd(214, 148, SPI_65x50, 65, 50, GREYY, BLACK);
	tft.print("Interface");
      }
    else
      {
	showbgd(214, 148, IIC_65x50, 65, 50, GREYY, BLACK);
	tft.print("Interface");
      }
  
    //control LED
    controlLED('0'); //off
    //control Logo
    controlLogo(GREYY);

    break;
        
      
    //eCO2 screen
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
    tehScreens[screenNr-3]->draw();
    break;

      //lightning screen after interrupt from sensor
      case 81:
        //print lightning information
        detectedlightningscreen();
        int tempScreenNr;
        tempScreenNr = previousScreenNr;  //store previous screen to show later
        previousScreenNr = 81;
        showScreen(tempScreenNr);   //after lightning pop-up, show previous screen
        break;

      // alarm clock screen

      default:
        //Serial.println("Nothing to see here folks!");
        break;
  }
  //store our current screen so we can track if we changed screens later
  previousScreenNr = currentScreenNr;
}


//print the screen with lightning info after interrupt
//also stores error messages so we can show them later
void detectedlightningscreen()
{
          tft.setFont();  //standard system font
          tft.setTextSize(2);
          tft.setCursor(35, 78);
          tft.setTextColor(RED,BLACK);
      
          //interpret interrupt source
          if (int_src == LIGHTNING_INT)
          {
            uint8_t lightning_dist_km;
            if(AS3935_SPI)
            {
                 lightning_dist_km = lightningSPI.distanceToStorm(); 
            }else
            {
                 lightning_dist_km = lightningIIC.distanceToStorm(); 
            }
            
            Serial.print("Lightning detected! Distance to strike: ");
            lastErrorLine1 = "Lightning detected!    ";
            lastErrorLine2 = "Distance to strike:";

#ifdef WITH_ESP01
	    i2mqtt("vm211/lightning", lightning_dist_km);
#endif
            if(MetricON)
            {
              Serial.print(lightning_dist_km);
              Serial.println("km");
              lastErrorLine2 += lightning_dist_km;
              lastErrorLine2 += "km ";
            }
            else
            {
              uint8_t lightning_dist_miles = lightning_dist_km / 1.609344;
              Serial.print(lightning_dist_miles);
              Serial.println("mi");
              lastErrorLine2 += lightning_dist_miles;
              lastErrorLine2 += "mi ";
            }
            
            tft.println(lastErrorLine1);
            tft.setCursor(35, 98);
            tft.println(lastErrorLine2);

            //report lightning energy, a pure number that does not have any physical meaning
            long lightEnergy;
            if(AS3935_SPI)
            {
                     lightEnergy = lightningSPI.lightningEnergy(); 
            }else
            {
                     lightEnergy = lightningIIC.lightningEnergy(); 
            }
            
            Serial.print("Lightning Energy: "); 
            Serial.println(lightEnergy); 
          }
          else if (int_src == DISTURBER_INT)
          {       
            Serial.println("Disturber detected!");
            lastErrorLine1 = "Disturber detected!    ";
            lastErrorLine2 = "                       ";
            tft.println(lastErrorLine1);
            tft.setCursor(35, 98);
            tft.println(lastErrorLine2);
          }
          else if (int_src == NOISE_INT)
          {
            Serial.println("Noise detected! Move the sensor.");
            lastErrorLine1 = "Noise detected!  ";
            lastErrorLine2 = "Please move the sensor.";
            tft.println(lastErrorLine1);
            tft.setCursor(35, 98);
            tft.println(lastErrorLine2);
          }
          else
          {
            Serial.print("IRQ source result "); Serial.print(int_src); Serial.println(" not expected...");
            lastErrorLine1 = "Error:IRQ source result";
            lastErrorLine2 = "  ";
            lastErrorLine2 += int_src;
            lastErrorLine2 += "   not expected...  ";
            tft.println(lastErrorLine1);
            tft.setCursor(35, 98);
            tft.println(lastErrorLine2);
          }
          
          //print icon
          showbgd(210, 125, lightning_100x77, 100, 77, YELLOW, BLACK);

          //print last detection
          tft.setCursor(35, 145);
          tft.setTextColor(YELLOW,BLACK);
          tft.println("Last detection:");
          tft.setCursor(35, 165);
          printLastDetectionTimeAS3935();
          AS3935IrqTriggeredTime = millis();  //set previous detection to now
      
          //buzz the buzzer for a short period + control the LED (if they are turned on)
          if(BuzzerEnabled){digitalWrite(BuzzerPin, HIGH);}
          controlLED('R');
          controlLogo(RED);
          Alarm.delay(100);
          if(BuzzerEnabled){digitalWrite(BuzzerPin, LOW);}
          controlLED('Y');
          controlLogo(YELLOW);
          Alarm.delay(200);
          if(BuzzerEnabled){digitalWrite(BuzzerPin, HIGH);}
          controlLED('R');
          controlLogo(RED);
          Alarm.delay(100);
          if(BuzzerEnabled){digitalWrite(BuzzerPin, LOW);}
          controlLED('Y');
          controlLogo(YELLOW);
          
          Alarm.delay(3000); //keep this info on screen for 3 seconds
}


//setup lighting sensor 
void setupAS3935()
{
    // Set everything to default values
    if(AS3935_SPI)
    {
              lightningSPI.resetSettings();
    }else
    {
             lightningIIC.resetSettings();
    }
    Alarm.delay(500);
    
    //read value for AS3935_OUTDOORS out of EEPROM memory
    AS3935_OUTDOORS = EEPROM.read(AS3935_outdoor_EEPROMaddr);
    //Serial.print("AS3935_OUTDOORS = "); Serial.println(AS3935_OUTDOORS);
    // The lightning detector defaults to an outdoor setting (more gain/sensitivity), but this can be modified via the setup page in the GUI
    int enviVal;
    if(AS3935_OUTDOORS)
    {
      if(AS3935_SPI)
      {
              lightningSPI.setIndoorOutdoor(OUTDOOR);
              enviVal = lightningSPI.readIndoorOutdoor();
      }else
      {
              lightningIIC.setIndoorOutdoor(OUTDOOR);
              enviVal = lightningIIC.readIndoorOutdoor();
      }
    }
    else
    {
      if(AS3935_SPI)
      {
              lightningSPI.setIndoorOutdoor(INDOOR);
              enviVal = lightningSPI.readIndoorOutdoor();
      }else
      {
              lightningIIC.setIndoorOutdoor(INDOOR);
              enviVal = lightningIIC.readIndoorOutdoor();
      }
      
    }
    //report current indoor/outdoor setting:
    Serial.print("Current setting (indoor/outdoor): ");  
    if( enviVal == INDOOR )  Serial.println("Indoor");  
    else if( enviVal == OUTDOOR ) Serial.println("Outdoor");  
    else Serial.println(enviVal, BIN); 

    // "Disturbers" are events that are false lightning events. If you notice a lot of disturbers, you can have the chip not report those events on the interrupt lines. 
    int maskVal;
    if(AS3935_SPI)
    {
            lightningSPI.maskDisturber(maskDisturbers); 
            maskVal = lightningSPI.readMaskDisturber();
    }else
    {
            lightningIIC.maskDisturber(maskDisturbers); 
            maskVal = lightningIIC.readMaskDisturber();
    }
    Serial.print("Are disturbers being masked: "); 
    if (maskVal == 1) Serial.println("YES"); 
    else if (maskVal == 0) Serial.println("NO"); 

    //Noise floor setting from 1-7, one being the lowest. Default setting is two.     
    int noiseVal;
    if(AS3935_SPI)
    {
            lightningSPI.setNoiseLevel(noiseFloor);
            noiseVal = lightningSPI.readNoiseLevel();
    }else
    {
            lightningIIC.setNoiseLevel(noiseFloor);
            noiseVal = lightningIIC.readNoiseLevel();
    }
    Serial.print("Noise Level is set at: ");
    Serial.println(noiseVal);

    // Watchdog threshold setting can be from 1-10, one being the lowest. Default setting is two.  
    int watchVal;
    if(AS3935_SPI)
    {
            lightningSPI.watchdogThreshold(watchDogVal); 
            watchVal = lightningSPI.readWatchdogThreshold();
    }else
    {
            lightningIIC.watchdogThreshold(watchDogVal); 
            watchVal = lightningIIC.readWatchdogThreshold();
    }   
    Serial.print("Watchdog Threshold is set to: ");
    Serial.println(watchVal);

    // Spike Rejection setting from 1-11, one being the lowest. Default setting is two. 
    // The shape of the spike is analyzed during the chip's validation routine. You can round this spike at the cost of sensitivity to distant events. 
    int spikeVal;
    if(AS3935_SPI)
    {
            lightningSPI.spikeRejection(spike); 
            spikeVal = lightningSPI.readSpikeRejection();
    }else
    {
            lightningIIC.spikeRejection(spike); 
            spikeVal = lightningIIC.readSpikeRejection();
    }    
    Serial.print("Spike Rejection is set to: ");
    Serial.println(spikeVal);

    // This setting will change when the lightning detector issues an interrupt. For example you will only get an interrupt after five lightning strikes instead of one. 
    // Default is one, and it takes settings of 1, 5, 9 and 16. Followed by its corresponding read function. Default is zero. 
    uint8_t lightVal;
    if(AS3935_SPI)
    {
            lightningSPI.lightningThreshold(lightningThresh); 
            lightVal = lightningSPI.readLightningThreshold();
    }else
    {
            lightningIIC.lightningThreshold(lightningThresh); 
            lightVal = lightningIIC.readLightningThreshold();
    }    
    Serial.print("The number of strikes before interrupt is triggerd: "); 
    Serial.println(lightVal); 

    // When the distance to the storm is estimated, it takes into account other lightning that was sensed in the past 15 minutes. 
    // If you want to reset time, then you can call this function. 
    if(AS3935_SPI)
    {
             lightningSPI.clearStatistics(1); 
    }else
    {
            lightningIIC.clearStatistics(1); 
    }
    
    Serial.println("AS3935 sensor has been set!");
}


//set the sensitivity of the AS3935 sensor
void updateLightningSense()
{
  globalSense = EEPROM.read(globalSense_EEPROMaddr);
  switch (globalSense) 
  {
    case 1:   //low sensitivity
      Serial.println("Setting AS3935 sensor to low sensitivity...");
      noiseFloor = 7;    // noise threshold value between 1-7, one being the lowest. Default setting is 2
      watchDogVal = 10;   // Watchdog threshold value between 1-10, one being the lowest. Default setting is 2
      spike = 9;         // Spike Rejection value between 1-11, one being the lowest. Default setting is 2. 
      lightningThresh = 5; //lightning threshold value, can be 1, 5, 9 and 16. Default setting is 1.
      maskDisturbers = true; //Mask disturbers. Values can be true or false. Default setting is true.
      break;
    case 2:   //medium sensitivity
      Serial.println("Setting AS3935 sensor to medium sensitivity...");
      noiseFloor = 4;    // noise threshold value between 1-7, one being the lowest. Default setting is 2
      watchDogVal = 5;   // Watchdog threshold value between 1-10, one being the lowest. Default setting is 2
      spike = 5;         // Spike Rejection value between 1-11, one being the lowest. Default setting is 2. 
      lightningThresh = 1; //lightning threshold value, can be 1, 5, 9 and 16. Default setting is 1.
      maskDisturbers = true; //Mask disturbers. Values can be true or false. Default setting is true.
      break;
    case 3:   //high sensitivity
      Serial.println("Setting AS3935 sensor to high sensitivity...");
      noiseFloor = 1;    // noise threshold value between 1-7, one being the lowest. Default setting is 2
      watchDogVal = 1;   // Watchdog threshold value between 1-10, one being the lowest. Default setting is 2
      spike = 2;         // Spike Rejection value between 1-11, one being the lowest. Default setting is 2. 
      lightningThresh = 1; //lightning threshold value, can be 1, 5, 9 and 16. Default setting is 1.
      maskDisturbers = false; //Mask disturbers. Values can be true or false. Default setting is true.
      break;
    default:
      Serial.println("Unknown sensitivity level for AS3935 sensor requested!");
      Serial.println("Reset level to medium sensitivity...");
      globalSense = 2;
      noiseFloor = 4;    // noise threshold value between 1-7, one being the lowest. Default setting is 2
      watchDogVal = 5;   // Watchdog threshold value between 1-10, one being the lowest. Default setting is 2
      spike = 5;         // Spike Rejection value between 1-11, one being the lowest. Default setting is 2. 
      lightningThresh = 1; //lightning threshold value, can be 1, 5, 9 and 16. Default setting is 1.
      maskDisturbers = true; //Mask disturbers. Values can be true or false. Default setting is true.
      break;
  }
}


//prints sensor information to screen (only used in info screen)
void printValues()
{
            tft.setFont();  //standard system font
            tft.setTextSize(1);
            
            //Print values, deleting those previously written by using a black background color
            //CO2
            tft.setCursor(50, 125);
            if (CO2 < 800)
            {
              tft.setTextColor(GREEN,BLACK);
            }
            else if ((CO2 >=800) && (CO2 < 1000))
            {
              tft.setTextColor(BLUE,BLACK);
            }
            else if ((CO2 >= 1000) && (CO2 < 1500))
            {
              tft.setTextColor(YELLOW,BLACK);
            }
            else if (CO2 >= 1500)
            {
              tft.setTextColor(RED,BLACK);
            }
      
            if(CO2 < 10)          {  tft.print("   "); }    //add leading spaces
            else if(CO2 < 100)    {  tft.print("  ");  }
            else if(CO2 < 1000)   {  tft.print(" ");   }
            tft.print(CO2);
            tft.println(" ppm");
      
            //TVOC
            tft.setCursor(50, 200);
            if (TVOC <= 50)
            {
              tft.setTextColor(GREEN,BLACK);
            }
            else if ((TVOC > 50) && (TVOC <= 150))
            {
              tft.setTextColor(YELLOW,BLACK);
            }
            else if (TVOC > 150)
            {
              tft.setTextColor(RED,BLACK);
            }
            
            if(TVOC < 10){          tft.print("   ");  }   //add leading spaces
            else if(TVOC < 100){    tft.print("  ");   }
            else if(TVOC < 1000){   tft.print(" ");    }
            tft.print(TVOC);
            tft.println(" ppb");
      
            //Temperature
            tft.setCursor(140, 125);
            if(TEMP_BME280 < 0)
            {
              tft.setTextColor(GREY,BLACK);
            }
            else if (TEMP_BME280 <= 18)
            {
              tft.setTextColor(BLUE,BLACK);
            }
            else if ((TEMP_BME280 > 18) && (TEMP_BME280 <= 24))
            {
              tft.setTextColor(GREEN,BLACK);
            }
            else if ((TEMP_BME280 > 24) && (TEMP_BME280 <= 30))
            {
              tft.setTextColor(YELLOW,BLACK);
            }
            else if (TEMP_BME280 > 30)
            {
              tft.setTextColor(RED,BLACK);
            }

            if(MetricON)
            {
               if(TEMP_BME280 < 10){   tft.print(" ");  }     //add leading spaces
               tft.print(TEMP_BME280,1);
               tft.print(" ");
               tft.print((char)247); //to print the ° symbol
               tft.println("C  ");
            }
            else
            {
               float TEMP_BME280_F = ((TEMP_BME280 * 9)/5 + 32);
               if(TEMP_BME280_F < 10){   tft.print(" ");  }     //add leading spaces
               tft.print(TEMP_BME280_F,1);
               tft.print(" ");
               tft.print((char)247); //to print the ° symbol
               tft.println("F");
            }
      
            //Humidity
            tft.setCursor(140, 200);
            if (HUMIDITY_BME280 < 30)
            {
              tft.setTextColor(RED,BLACK);
            }
            else if ((HUMIDITY_BME280 >= 30) && (HUMIDITY_BME280 <= 50))
            {
              tft.setTextColor(GREEN,BLACK);
            }
            else if (HUMIDITY_BME280 > 50)
            {
              tft.setTextColor(BLUE,BLACK);
            }
            if(HUMIDITY_BME280 < 10) { tft.print(" ");   }  //add leading spaces
            tft.print(HUMIDITY_BME280,1);
            tft.println(" %");
      
            //Pressure
            tft.setCursor(215, 125);
            tft.setTextColor(GREY,BLACK);
            if(AMBIENTPRESSURE_BME280_c < 1000)
            {
              tft.print(" ");     //add leading spaces
            }
            tft.print(AMBIENTPRESSURE_BME280_c,1);
            tft.println(" mBar");

            //print last detection time lightning sensor
            tft.setCursor(210, 200);
            printLastDetectionTimeAS3935();
            
            //print total quality
            tft.setCursor(125, 225);
            Print_Level(CO2);
}


//print menu buttons, according to current screen
void printMenuBtn()
{
    if(currentScreenNr == 0)
    {
      //print nothing, boot menu
    }
    else if(currentScreenNr == 1) //info screen
    {
      //print slide show control button
      if(slideShowPlaying)
      {
        showbgd(10, 210, play_23x23, 23, 23, GREYY, BLACK);
      }
      else
      {
        showbgd(10, 210, pauze_23x23, 23, 23, GREYY, BLACK);
      }
      //print setup button
      showbgd(287, 210, setup_23x23, 23, 23, GREYY, BLACK);
    }
    else if(currentScreenNr == 2) //setup screen
    {
      //show menu instead of setup icon
      showbgd(287, 210, menu_23x23, 23, 23, GREYY, BLACK);
    }
    else if(currentScreenNr == 81) //lightning screen after interrupt
    {
      //only show info icon, because no user input is allowed
      showbgd(10, 210, info_23x23, 23, 23, RED, BLACK);
    }
    else  //other screens
    {
      //print slideshow control button
      if(slideShowPlaying)
      {
        showbgd(10, 210, play_23x23, 23, 23, GREYY, BLACK);
      }
      else
      {
        showbgd(10, 210, pauze_23x23, 23, 23, GREYY, BLACK);
      }
      //print menu screen button
      showbgd(287, 210, menu_23x23, 23, 23, GREYY, BLACK);
      //print location circles

      // this has slightly different behavior if the clock is used or not
      for(int i=0; i<(MAXSCREEN-2); i++)
      {
        int xPosCircle = 110 + (i * 20)-((MAXSCREEN==9)?10:0);
        tft.fillCircle(xPosCircle, 221, 7, GREYY);   //fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
      }
      //fill currentscreen circle
      int xPosCircleSelected = 110 + ((currentScreenNr - 3) * 20)-((MAXSCREEN==9)?10:0);
      tft.fillCircle(xPosCircleSelected, 221, 5, BLACK);   //fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
    }
}


//looks at minutes since AS3935 trigger and outputs in none/min/hours/>1day to screen
void printLastDetectionTimeAS3935()
{
  if(AS3935IrqTriggeredTime == 0) //still set to init value -> no detection recorded!
  {
    if(AS3935_bootOK)
    {
      tft.setTextColor(GREEN,BLACK);
      tft.print("    none    ");
      if(currentScreenNr == 8){controlLED('G');controlLogo(GREEN);};
    }
    else
    {
      tft.setTextColor(RED,BLACK);
      tft.print("   error    ");
      if(currentScreenNr == 8){controlLED('R');controlLogo(RED);};
    }
  }
  else if(minutesSinceAS3935Trigger < 1)
  {
    tft.setTextColor(RED,BLACK);
    tft.print("< 1 min ago ");
    if(currentScreenNr == 8){controlLED('R');controlLogo(RED);};
  }
  else if(minutesSinceAS3935Trigger > 60)
  {
    int hoursSinceAS3935Trigger = minutesSinceAS3935Trigger / 60;
    tft.setTextColor(GREEN,BLACK);
    if(currentScreenNr == 8){controlLED('G');controlLogo(GREEN);};
    if(hoursSinceAS3935Trigger > 24)
    {
      tft.print("> 1 day ago ");
    }
    else
    {
      if(hoursSinceAS3935Trigger < 10)
      {
        tft.print(" "); //fix leading space
      }
      tft.print(hoursSinceAS3935Trigger);
      tft.print(" hours ago");
    }
  }
  else
  {
    if(minutesSinceAS3935Trigger <= 15)
    {
      tft.setTextColor(RED,BLACK);
      if(currentScreenNr == 8){controlLED('R');controlLogo(RED);};
    }
    else if((minutesSinceAS3935Trigger > 15) && (minutesSinceAS3935Trigger <= 30))
    {
      tft.setTextColor(YELLOW,BLACK); 
      if(currentScreenNr == 8){controlLED('Y');controlLogo(YELLOW);};
    }
    else
    {
      tft.setTextColor(GREEN,BLACK); 
      if(currentScreenNr == 8){controlLED('G');controlLogo(GREEN);};
    }
    if(minutesSinceAS3935Trigger < 10)
    {
      tft.print(" "); //fix leading space
    }
    tft.print(minutesSinceAS3935Trigger);
    tft.print(" min ago  ");
  }
}


//analyses the touch input (according to current screen) and links to other screens/functions
void inputControl()
{ 
    //first, check on which screen we are, then rule out what to do and which screen to go to
    switch (currentScreenNr) 
    {
      //boot screen
      case 0:
        // no touch functions
        break;
        
      //info screen
      case 1:
          //check slide show button
          if( (Xpos > 0) && (Xpos < 45) && (Ypos > 0) && (Ypos < 30) )
          {
            //Serial.println("Touch on slide show button!");
            toggleSlideShow();
            previousScreenNr = currentScreenNr;
            currentScreenNr = 3;    //start slide show on screen 3
          }
          //check setup screen button
          else if( (Xpos > 0) && (Xpos < 45) && (Ypos > 210) && (Ypos < 245) )
          {
            //Serial.println("Touch on setup screen button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 2;
          }
          //check eCO2 button
          else if( (Xpos > 140) && (Xpos < 220) && (Ypos > 30) && (Ypos < 80) )
          {
            //Serial.println("Touch on eCO2 button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 3;
          }
          //check temp button
          else if( (Xpos > 140) && (Xpos < 220) && (Ypos > 100) && (Ypos < 140) )
          {
            //Serial.println("Touch on temp button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 4;
          }
          //check pressure button
          else if( (Xpos > 140) && (Xpos < 220) && (Ypos > 165) && (Ypos < 210) )
          {
            //Serial.println("Touch on pressure button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 5;
          }
          //check tvoc button
          else if( (Xpos > 50) && (Xpos < 110) && (Ypos > 30) && (Ypos < 80) )
          {
            //Serial.println("Touch on tvoc button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 6;
          }
          //check humidity button
          else if( (Xpos > 50) && (Xpos < 110) && (Ypos > 100) && (Ypos < 140) )
          {
            //Serial.println("Touch on humidity button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 7;
          }
          //check lightning button
          else if( (Xpos > 50) && (Xpos < 110) && (Ypos > 165) && (Ypos < 210) )
          {
            //Serial.println("Touch on lightning button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 8;
          }
          else
          {
            //Serial.println("I do not know what you are touching...");
          }
        break;
        
      //setup screen
      case 2:
        //check info screen button
        if( (Xpos > 0) && (Xpos < 40) && (Ypos > 215) && (Ypos < 245) )
        {
          //Serial.println("Touch on info screen button!");
          previousScreenNr = currentScreenNr;
          currentScreenNr = 1;
        }
        //check speaker button
        else if( (Xpos > 140) && (Xpos < 220) && (Ypos > 30) && (Ypos < 80) )
        {
          //Serial.println("Touch on speaker button!");
          toggleBuzzer();
        }
        //check metric/imperial button
        else if( (Xpos > 140) && (Xpos < 220) && (Ypos > 100) && (Ypos < 140) )
        {
          //Serial.println("Touch on metric/imperial button!");
          toggleMetric();
        }
        //check timer button
        else if( (Xpos > 140) && (Xpos < 220) && (Ypos > 165) && (Ypos < 210) )
        {
          //Serial.println("Touch on timer button!");
          slideshowTimer = slideshowTimer + 5;
          if(slideshowTimer > 60){slideshowTimer = 5;}
        }
        //check sensitivity button
        else if( (Xpos > 50) && (Xpos < 110) && (Ypos > 30) && (Ypos < 80) )
        {
          //Serial.println("Touch on sensitivity button!");
          if(AS3935_bootOK)  //only when sensor is up and running can we change the value
          {
            globalSense++;
            if(globalSense > 3){globalSense = 1;}
            //write updated value to EEPROM long term memory
            EEPROM.write(globalSense_EEPROMaddr, globalSense);
            updateLightningSense(); //set the sensitivity of the sensor
            setupAS3935();  //run setup of AS3935 sensor
          }
          else
          {
            //push warning message
            tft.setFont();  //standard system font
            tft.setTextSize(1);
            tft.setTextColor(RED,BLACK); 
            tft.setCursor(20, 220);
            tft.print("Sensor error: check wiring & interface");
          }
        }
        //check indoor/outdoor button
        else if( (Xpos > 50) && (Xpos < 110) && (Ypos > 100) && (Ypos < 140) )
        {
          //Serial.println("Touch on indoor/outdoor button!");
          toggleIndoor();
        }
        //check interface button
        else if( (Xpos > 50) && (Xpos < 110) && (Ypos > 165) && (Ypos < 210) )
        {
          //Serial.println("Touch on interface button!");
          toggleInterface();
        }
        else
        {
          //Serial.println("No idea what you are touching there buddy...");
        }
        break;
        
      //eCO² screen
      case 3:
        checkBaseTouch();
        break;
        
      //temperature screen
      case 4:
        checkBaseTouch();
        break;
        
      //pressure screen
      case 5:
        checkBaseTouch();
        break;
        
      //TVOC screen
      case 6:
        checkBaseTouch();
        break;

      //humidity screen
      case 7:
        checkBaseTouch();
        break;
        
      //lightning screen
      case 8:
        checkBaseTouch();
        break;
        
      //lightning screen after interrupt
      case 81:
        // no touch functionality (delay of 3 seconds, so touch would never be handled correct)
        break;
        
      default:
        // no default statements
        break;
    }
}


//check for touches in detailed menus 
//these values were found by simply touching parts of the screen and defining borders
//there might be some logic, but I tried until it worked :-)
void checkBaseTouch()
{
  //check slide show button
  if( (Xpos > 0) && (Xpos < 45) && (Ypos > 0) && (Ypos < 30) )
  {
    //Serial.println("Touch on slide show button!");
    toggleSlideShow();
    previousScreenNr = currentScreenNr;
    currentScreenNr++;    //go to the next slide immediately
    if(currentScreenNr > MAXSCREEN){currentScreenNr = 3;}
  }
  //check info screen button
  else if( (Xpos > 0) && (Xpos < 45) && (Ypos > 210) && (Ypos < 245) )
  {
    //Serial.println("Touch on info screen button!");
    previousScreenNr = currentScreenNr;
    currentScreenNr = 1;
    slideShowPlaying = 0;
  }
  //check input left side of screen
  else if( (Xpos > 50) && (Xpos < 300) && (Ypos > 0) && (Ypos < 100) )
  {
    //Serial.println("Touch on left side of screen!");
    previousScreenNr = currentScreenNr;
    currentScreenNr--;
    if(currentScreenNr < 3){currentScreenNr = MAXSCREEN;}
    slideShowPlaying = 0;
  }
  //check input right side of screen
  else if( (Xpos > 50) && (Xpos < 300) && (Ypos > 140) && (Ypos < 245) )
  {
    //Serial.println("Touch on right side of screen!");
    previousScreenNr = currentScreenNr;
    currentScreenNr++;
    if(currentScreenNr > MAXSCREEN){currentScreenNr = 3;}
    slideShowPlaying = 0;
  }
  //check input circles middle of screen
  else if( (Xpos > 0) && (Xpos < 40) && (Ypos > 80) && (Ypos < 165) )
  {
    //Serial.println("Touch on middle bottom of screen!");
    previousScreenNr = currentScreenNr;
    currentScreenNr++;
    if(currentScreenNr > MAXSCREEN){currentScreenNr = 3;}
    slideShowPlaying = 0;
  }
  else
  {
    //Serial.println("I have no idea what you are trying to touch...");
  }
}


//check state of Slide show & toggle
void toggleSlideShow()
{
  if(slideShowPlaying) { slideShowPlaying = 0; } else { slideShowPlaying = 1; }
  printMenuBtn(); //reprint button
}


//check state of Buzzer & toggle + write to long term memory (so if you loose power, we still know what you want)
void toggleBuzzer()
{
  if(BuzzerEnabled) { BuzzerEnabled = 0; } else { BuzzerEnabled = 1; }
  //write updated value to EEPROM long term memory
  EEPROM.write(Buzzer_EEPROMaddr, BuzzerEnabled);
}


//check state of MetricON
void toggleMetric()
{
  if(MetricON) { MetricON = false; } else { MetricON = true; }
  EEPROM.write(MetricON_EEPROMaddr, MetricON);
}


//check status of indoor mode & toggle + write to long term memory (so if you loose power, we still know what you want)
void toggleIndoor()
{
  if(AS3935_OUTDOORS)
  { 
    AS3935_OUTDOORS = 0;
    if(AS3935_SPI)
    {
             lightningSPI.setIndoorOutdoor(INDOOR); 
    }else
    {
            lightningIIC.setIndoorOutdoor(INDOOR); 
    }
  } 
  else 
  { 
    AS3935_OUTDOORS = 1; 
    if(AS3935_SPI)
    {
             lightningSPI.setIndoorOutdoor(OUTDOOR);
    }else
    {
            lightningIIC.setIndoorOutdoor(OUTDOOR);
    }
  }
  //write updated value to EEPROM long term memory
  EEPROM.write(AS3935_outdoor_EEPROMaddr, AS3935_OUTDOORS);
}


//change interface from SPI to IIC or IIC to SPI - reboot required
void toggleInterface()
{
  if(AS3935_SPI){ AS3935_SPI = false; } else { AS3935_SPI = true; }
  //write updated value to EEPROM long term memory
  EEPROM.write(AS3935_SPI_EEPROMaddr, AS3935_SPI);
  //push reboot message
  tft.setFont();  //standard system font
  tft.setTextSize(1);
  tft.setTextColor(RED,BLACK); 
  tft.setCursor(20, 220);
  tft.print("   Interface changed, please reboot!  ");
  Alarm.delay(800);
}


// Draws a bitmap on the screen, works way faster then drawBitmap()!
// showbgd(start position X, start position y, bitmap, width, height, color, background color)
// GFX drawBitmap() expects each row to be on 8-bit boundary.  i.e. pad = 7
// Mono BMP rows are on 32-bit boundary. i.e. pad = 31,  rows are in RVS order.
// FreeFont bitmaps have no padding.  i.e. pad = 0.   e.g. 5x4 bitmap is in 3 bytes
void showbgd(int x, int y, const uint8_t *bmp, int w, int h, uint16_t color, uint16_t bg)
{
    uint8_t pad = 7;
    int yy, xx;
    uint8_t first = 1, RVS = pad & 0x80;
    uint16_t pixel;
    pad &= 31;
    uint16_t wid = (w + pad) & ~pad;                    //bits per row
    tft.setAddrWindow(x, y, x + w - 1, y + h - 1); //
    for (yy = 0; yy < h; yy++) {
        uint32_t ofs = (RVS ? (h - yy - 1) : yy) * wid; //bit offset
        const uint8_t *p = bmp + (ofs >> 3);            //flash address
        uint8_t mask = 0x80 >> (ofs & 7);               //bit mask
        uint8_t c = pgm_read_byte(p++);
        for (xx = 0; xx < w; xx++) {
            if (mask == 0) {
                c = pgm_read_byte(p++);
                mask = 0x80;
            }
            pixel = (c & mask) ? color : bg;
            tft.pushColors(&pixel, 1, first);
            first = 0;
            mask >>= 1;
        }
    }
    tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1);
}

// -- i2c scanner, from https://playground.arduino.cc/Main/I2cScanner/
// -- for debugging of i2c connections. 

void scani2c() {
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}
