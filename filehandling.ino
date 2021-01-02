// Functions to manipulate the contents of the SD card (if present)
//
// (C) 2020 Kristian Wiklund - except for the (minor) reuse of Velleman code, notably the csv header writer.
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

// take the data from the sensors. Write to file, if sd card is present
void writecsv() {

  if(!SDpresent)
    return;

  File fd = SD.open(LOGFILE, FILE_WRITE);
  
  if(!logFileExists) {
    Serial.println("SD: Initializing log file");
    // we will actually break velleman compatibility here and log wallclock time if we have a clock source
    if(clockscreen.isEnabled()) 
      fd.println("Date,Time,Temperature [°C],Humidity [%],Pressure [mBar],Altitude [m],eCO2 [ppm],TVOC [ppb]"); // date and time will be stored in this format: "2020-08-13,18:34:10", which excel is able to parse
    else      
      fd.println("Time since boot [DD HH:MM:SS],Temperature [°C],Humidity [%],Pressure [mBar],Altitude [m],eCO2 [ppm],TVOC [ppb]");
  }

  // write data for each sensor here. empty string if no sensor exists

  fd.close();
    
  
}
