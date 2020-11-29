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


