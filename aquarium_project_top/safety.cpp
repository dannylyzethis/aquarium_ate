#include <Arduino.h>
#include "config.h"
#include "safety.h"
#include "sensors.h"
#include "actuators.h"
#include "blynk.h"

bool failSafeMode = false;
String faultMessage = "";

void setupSafety() {
  pinMode(LEAK_SENSOR_PIN, INPUT);
  pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
}

void checkSafetyConditions(unsigned long currentTime) {
  if (digitalRead(LEAK_SENSOR_PIN) == HIGH) {
    failSafeMode = true;
    faultMessage = "Leak Detected";
    disableAllPumps();
    sendAlert("Leak Detected");
  }
  if (tankTemp > MAX_WATER_TEMP || secondaryTemp > MAX_WATER_TEMP) {
    failSafeMode = true;
    faultMessage = "Overheat Detected";
    disableAllPumps();
    sendAlert("Overheat Detected");
  }
  if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
    failSafeMode = true;
    faultMessage = "Emergency Stop";
    disableAllPumps();
    sendAlert("Emergency Stop Activated");
  }
  if (drainPumpState && (currentTime - drainStartTime > DRAIN_TIMEOUT)) {
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;
    faultMessage = "Drain Timeout";
    sendAlert("Drain Timeout");
  }
  // Add similar checks for fill, top-off, and RO/DI if needed
}