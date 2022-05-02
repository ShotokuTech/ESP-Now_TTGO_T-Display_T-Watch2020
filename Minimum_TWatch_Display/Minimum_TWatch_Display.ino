#include "config.h"
TTGOClass *ttgo;
TFT_eSPI *tft;
void setup()
{
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    tft = ttgo->tft;
    tft->fillScreen(TFT_BLACK);
    tft->setTextFont(2);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
}
void loop()
{
  int count = 0;
  tft->setCursor(0, 0);
  while(count<14){
  tft->println("Hello World ");
  count++;
  }
  delay(1000);
  tft->fillScreen(TFT_BLACK);
}
