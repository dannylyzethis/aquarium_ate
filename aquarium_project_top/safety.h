#ifndef SAFETY_H
#define SAFETY_H

#include <Arduino.h>

extern bool failSafeMode;
extern String faultMessage;

void setupSafety();
void checkSafetyConditions(unsigned long currentTime);
void buzzerAlert();
float readCurrent(int pin);

#endif