#include <Adafruit_ILI9341.h>
#include "config.h"
#include "display.h"
#include "sensors.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setupDisplay() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
}

void updateDisplay() {
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("Tank Temp: ");
  tft.print(tankTemp);
  tft.println(" C");
  tft.print("Barrel Temp: ");
  tft.print(barrelTemp);
  tft.println(" C");
  tft.print("pH: ");
  tft.println(pH);
  if (SALTWATER_MODE) {
    tft.print("Tank Salinity: ");
    tft.println(tankSalinity);
    tft.print("Barrel Salinity: ");
    tft.println(barrelSalinity);
  }
}
