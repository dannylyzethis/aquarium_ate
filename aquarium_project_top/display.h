#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>

extern int currentScreen;

void setupDisplay();
void updateDisplay();
void drawHomeScreen();
void drawControlScreen();
void handleTouch();

#endif