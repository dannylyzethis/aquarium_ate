#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
float readSalinity(int address);
extern float tankTemp, barrelTemp, secondaryTemp, pH, tds, turbidity, roomTemp, humidity;
extern float tankSalinity, barrelSalinity;
extern String tankLevel, barrelLevel;


void setupSensors();
void updateSensors();

#endif
