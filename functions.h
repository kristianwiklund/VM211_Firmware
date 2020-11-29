/* fun.c */
void Print_Level(int data1);
void printSensorError(void);
unsigned long testFillScreen(void);
void updateCCS811vars(float TEMP, float HUMIDITY);
void interruptFunction(void);
void controlLED(char COLOR);
void getTimeSinceBoot(void);
int returnMinutes(unsigned long Millis);
void showScreen(int screenNr);
void oldlightningscreen(void);
void setupAS3935(void);
void updateLightningSense(void);
void printValues(void);
void printMenuBtn(void);
void printLastDetectionTimeAS3935(void);
void inputControl(void);
void checkBaseTouch(void);
void toggleSlideShow(void);
void toggleBuzzer(void);
void toggleMetric(void);
void toggleIndoor(void);
void toggleInterface(void);
void scani2c(void);
void showbgd(int x, int y, const uint8_t *bmp, int w, int h, uint16_t color, uint16_t bg);
void controlLogo(uint16_t logocolor);
  
