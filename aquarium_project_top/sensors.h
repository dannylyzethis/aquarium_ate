#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

extern float tankTemp, barrelTemp, secondaryTemp, pH, tds, turbidity, roomTemp, humidity, salinity;
extern String tankLevel, barrelLevel;

void setupSensors();
void updateSensors();

#endif