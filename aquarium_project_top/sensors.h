#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

extern float tankTemp, barrelTemp, secondaryTemp, pH, tds, turbidity, roomTemp, humidity;
#if SALTWATER_MODE
extern float tankSalinity, barrelSalinity;
#endif
extern String tankLevel, barrelLevel;
float readSalinity(int address);

void setupSensors();
void updateSensors();

#endif