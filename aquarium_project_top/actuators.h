#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include "watchdog_compat.h"

extern bool drainPumpState, fillPumpState, topOffPumpState, rodiValveState;
extern unsigned long drainStartTime, fillStartTime, topOffStartTime, rodiStartTime;

void setupActuators();
void disableAllPumps();
void performWaterChange();

#endif
