#include "config.h"
#include "functions.h"
// classes to handle the graphics screens

class Screen {

  bool isenabled=false;
  
public:

  bool isEnabled();   
  void draw();
  
};

class BootScreen : public Screen {
public:
  void draw();
};


