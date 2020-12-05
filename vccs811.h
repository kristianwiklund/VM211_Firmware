#ifndef vCCS811_H
#define vCCS811_H

#include <SparkFunCCS811.h> //Click here to get the library: http://librarymanager/All#SparkFun_CCS811

extern uint16_t CO2;
extern uint16_t TVOC;
extern CCS811 myCCS811;


void ccs811_setup();
void ccs811_loop();


#endif
