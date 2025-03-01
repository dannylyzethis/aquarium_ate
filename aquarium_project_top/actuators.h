#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>

extern bool drainPumpState;
extern bool fillPumpState;
extern bool topOffPumpState;
extern bool rodiValveState;
extern unsigned long drainStartTime;
extern unsigned long fillStartTime;
extern unsigned long topOffStartTime;
extern unsigned long rodiStartTime;

void setupActuators();
void disableAllPumps();
void performWaterChange();

#endif