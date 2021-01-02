// Functions to manipulate the contents of the SD card (if present)
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

// this file will handle three things:
// (1) detect if the SD card is present
// (2) write telemetery to the SD card compatible with the Velleman solution
// (3) read and write alarm clock config from the card


// read alarm configuration from file
void readconfig() {

}


bool detectSD() {

  // library requirement: the "normal" select pin must be in output mode even if not used.
  pinMode(SS, OUTPUT);
  
  SDpresent = SD.begin(SD_CHIP_SELECT);

  if(SDpresent) {
    Serial.println(" card initialized!");
    SDpresent = 1;
    
    //Check if our logfile is already on the card
    if(SD.exists(LOGFILE))
      {
	Serial.print("Logfile present on card");
	logFileExists = 1;
      }

    //Check if our logfile is already on the card
    if(SD.exists(CONFIGFILE))
      {
	Serial.print("Configfile present on card");
	readconfig();
      }
  }
  else
    SDpresent = 0;
  
  return (SDpresent);
  
}

// ***** start of the original SD writer code by Velleman
#if 0
      //write dataString to SD (if SD card is present & we have passed the interval to log)
      if(SDpresent)
	{

	  File dataFile = SD.open(logFileName, FILE_WRITE);
	  // open the file. note that only one file can be open at a time,
	  // so you have to close this one before opening another.
           
	  // if the file is available, write to it:
	  if (dataFile) 
	    {
	      if(!logFileExists)  //the logfile didn't exist, so first print headers
		{
		  Serial.print("Logfile '");
		  Serial.print(logFileName);
		  Serial.println("' did not exist, so print titles first..."); 
		  dataFile.println("Time since boot [DD HH:MM:SS],Temperature [°C],Humidity [%],Pressure [mBar],Altitude [m],eCO2 [ppm],TVOC [ppb]");
		  logFileExists = 1;
		}
	      dataFile.println(dataString);
	      dataFile.close();

	      // print to the serial port too:
	      Serial.print("Written to file ");
	      Serial.print(logFileName);
	      Serial.print(" on SD card: ");
	      Serial.println(dataString);
	    }
	  // if the file isn't open, pop up an error:
	  else 
	    {
	      Serial.print("Error opening file ");
	      Serial.print(logFileName);
	      Serial.println(" on SD card! No data logged.");
	    }
	}
#endif
// ***** end of the original SD writer code by Velleman
