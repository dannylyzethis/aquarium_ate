#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "sensors.h"
#include "safety.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int currentScreen = 0;

void setupDisplay() {
  tft.begin();
  tft.setRotation(1);
  drawHomeScreen();
}

void updateDisplay() {
  if (currentScreen == 0) {
    drawHomeScreen();
  } else if (currentScreen == 1) {
    drawControlScreen();
  }
}

void drawHomeScreen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);

  tft.setCursor(10, 10);
  if (failSafeMode) {
    tft.setTextColor(ILI9341_RED);
    tft.println("FAILSAFE: " + faultMessage);
  } else {
    tft.println("Aquarium Status");
  }
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(10, 40);
  tft.print("Tank Temp: "); tft.print(tankTemp, 1); tft.println(" C");

  tft.setCursor(10, 60);
  tft.print("Barrel Temp: "); tft.print(barrelTemp, 1); tft.println(" C");

  tft.setCursor(10, 80);
  tft.print("Equip Temp: "); tft.print(secondaryTemp, 1); tft.println(" C");

  tft.setCursor(10, 100);
  tft.print("Tank Level: "); tft.println(tankLevel);

  tft.setCursor(10, 120);
  tft.print("Barrel Level: "); tft.println(barrelLevel);

  tft.setCursor(10, 140);
  tft.print("pH: "); tft.print(pH, 1);

  tft.setCursor(10, 160);
  tft.print("TDS: "); tft.print(tds, 0); tft.println(" ppm");

  tft.setCursor(10, 180);
  tft.print("Room Temp: "); tft.print(roomTemp, 1); tft.println(" C");

  tft.setCursor(10, 200);
  tft.print("Humidity: "); tft.print(humidity, 1); tft.println(" %");

  #if SALINITY_MONITORING
    tft.setCursor(10, 220);
    tft.print("Salinity: "); tft.print(salinity, 1); tft.println(" ppt");
  #endif
}

void drawControlScreen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10, 10);
  tft.println("Control Screen");
}

void handleTouch() {
  if (failSafeMode) return;
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > ts.pressureThreshhold) {
    int16_t x = map(p.y, 240, 0, 0, 320);
    int16_t y = map(p.x, 0, 320, 0, 240);

    if (currentScreen == 0 && x > 280 && y < 40) {
      currentScreen = 1;
      drawControlScreen();
    } else if (currentScreen == 1 && x > 280 && y < 40) {
      currentScreen = 0;
      drawHomeScreen();
    }
  }
}