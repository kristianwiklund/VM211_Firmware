#include "config.h"
#include "functions.h"
// classes to handle the graphics screens

class Screen {

  bool isenabled=false;
  
public:

  bool isEnabled();
  void setEnabled(bool);
  virtual bool draw();
  
};

class BootScreen : public Screen {

  bool isdrawn=false;
  int state=0;
  
public:
  BootScreen();
  bool draw();
  void cycle();
  void clear();
};

class InfoScreen : public Screen {

public:
  InfoScreen();
  bool draw();
};

class CO2Screen : public Screen {

public:
  CO2Screen();
  bool draw();
};

class TVOCScreen : public Screen {

public:
  TVOCScreen();
  bool draw();
};

class TempScreen : public Screen {

public:
  TempScreen();
  bool draw();
};

class PressureScreen : public Screen {

public:
  PressureScreen();
  bool draw();
};

class HumidityScreen : public Screen {

public:
  HumidityScreen();
  bool draw();
};


class ClockScreen : public Screen {

public:
  ClockScreen();
  bool draw();
};

class LightningScreen : public Screen {

public:
  LightningScreen();
  bool draw();
};


