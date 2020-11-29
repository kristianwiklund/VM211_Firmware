#include "config.h"
#include "functions.h"
// classes to handle the graphics screens

class Screen {

  bool isenabled=false;
  
public:

  bool isEnabled();
  void setEnabled(bool);
  bool draw();
  
};

class BootScreen : public Screen {

public:
  BootScreen();
  bool draw();
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


