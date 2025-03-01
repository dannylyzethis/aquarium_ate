#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
float readSalinity(int address);
extern float tankTemp, barrelTemp, secondaryTemp, pH, tds, turbidity, roomTemp, humidity;
#if SALTWATER_MODE
extern float tankSalinity, barrelSalinity;
#endif
extern String tankLevel, barrelLevel;


void setupSensors();
void updateSensors();

#endif