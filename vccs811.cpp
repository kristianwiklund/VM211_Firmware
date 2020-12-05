
// for unknown reasons the original code for CCS811 did not work well. I got really high readings, like 2500-4000 ppm.
// using the sparkfun example sketch worked. Hence, these functions are derived from that sketch to be able to get some
// insight in what might be happening - the original code is less readable than I prefer

#include <Arduino.h>
#include <Wire.h>
#include <SparkFunCCS811.h> //Click here to get the library: http://librarymanager/All#SparkFun_CCS811
#include "config.h"
#include "screens.h"

#include "src/MCUFRIEND_kbv/MCUFRIEND_kbv.h"                  // TFT library by David Prentice
extern MCUFRIEND_kbv tft;


extern CCS811 myCCS811;

extern uint16_t CO2;
extern uint16_t TVOC;

extern CO2Screen co2screen;
extern TVOCScreen tvocscreen;


//printDriverError decodes the CCS811Core::status type and prints the
//type of error to the serial terminal.
//
//Save the return value of any function of type CCS811Core::status, then pass
//to this function to see what the output was.

void printDriverError( CCS811Core::CCS811_Status_e errorCode )
{
  tft.setCursor(15, 105);
  tft.setTextColor(WHITE); 
  tft.setTextSize(2);

  tft.print("CCS811 status: ");

  switch ( errorCode )
  {
    case CCS811Core::CCS811_Stat_SUCCESS:
      Serial.print("SUCCESS");
      tft.setTextColor(GREEN); 
      tft.print("DETECTED");
      co2screen.setEnabled(true);
      tvocscreen.setEnabled(true);
      return (true);
      break;
    case CCS811Core::CCS811_Stat_ID_ERROR:
      Serial.print("ID_ERROR");
      tft.setTextColor(RED); 
      tft.print("ID_ERROR");
      break;
    case CCS811Core::CCS811_Stat_I2C_ERROR:
      Serial.print("I2C_ERROR");
      tft.setTextColor(RED); 
      tft.print("I2C_ERROR");
      break;
    case CCS811Core::CCS811_Stat_INTERNAL_ERROR:
      Serial.print("INTERNAL_ERROR");
      tft.setTextColor(RED); 
      tft.print("INTERNAL_ERROR");
      break;
    case CCS811Core::CCS811_Stat_GENERIC_ERROR:
      Serial.print("GENERIC_ERROR");
      tft.setTextColor(RED); 
      tft.print("GENERIC_ERROR");
      break;
    default:
      Serial.print("Unspecified error.");
      tft.setTextColor(RED); 
      tft.print("Unspecified error.");
  }
  return false;
}


void ccs811_setup() {
  CCS811Core::CCS811_Status_e returnCode;
  Wire.begin(); //Inialize I2C Hardware

  returnCode = myCCS811.beginCore(Wire);
  Serial.print("CCS811 begin exited with: ");
  printDriverError( returnCode );
  Serial.println();
  


  return true;
}

// read from ccs811 and update the two lovely globals
void ccs811_loop()
{
  //Check to see if data is ready with .dataAvailable()
  if (myCCS811.dataAvailable())
  {
    //If so, have the sensor read and calculate the results.
    //Get them later
    myCCS811.readAlgorithmResults();

    Serial.print("CO2[");
    //Returns calculated CO2 reading
    CO2 = myCCS811.getCO2();
    Serial.print(CO2);
    Serial.print("] tVOC[");
    //Returns calculated TVOC reading
    TVOC = myCCS811.getTVOC();
    Serial.print(TVOC);
    Serial.print("] millis[");
    //Display the time since program start
    Serial.print(millis());
    Serial.print("]");
    Serial.println();
  }
}

