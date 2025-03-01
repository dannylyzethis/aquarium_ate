#include <Arduino.h>
#include "config.h"
#include "commands.h"
#include "sensors.h"
#include "actuators.h"
#include "safety.h"

void handleSerialCommands() {
  if (Serial2.available()) {
    String cmd = Serial2.readStringUntil('\n');
    handleSerialCommands(cmd);
  }
}

void handleSerialCommands(String cmd) {
  cmd.toUpperCase();
  if (cmd == "TEMP_TANK") {
    Serial2.println(String(tankTemp, 1));
  } else if (cmd == "TEMP_BARREL") {
    Serial2.println(String(barrelTemp, 1));
  } else if (cmd == "SALINITY" && SALINITY_MONITORING) {
    Serial2.println(String(salinity, 1));
  } else if (cmd == "STATUS") {
    Serial2.println("Drain Pump: " + String(drainPumpState ? "ON" : "OFF"));
    Serial2.println("Fill Pump: " + String(fillPumpState ? "ON" : "OFF"));
    Serial2.println("Top-Off Pump: " + String(topOffPumpState ? "ON" : "OFF"));
    Serial2.println("RO/DI Valve: " + String(rodiValveState ? "ON" : "OFF"));
    Serial2.println("Fail-Safe: " + String(failSafeMode ? "ON" : "OFF"));
  } else if (cmd == "RESET_FAILSAFE" && failSafeMode) {
    failSafeMode = false;
    faultMessage = "";
    Serial2.println("Fail-safe mode reset");
  } else if (cmd == "DRAIN_ON" && !failSafeMode) {
    digitalWrite(DRAIN_PUMP_PIN, LOW);
    drainPumpState = true;
    drainStartTime = millis();
    Serial2.println("Drain pump ON");
  } else if (cmd == "DRAIN_OFF") {
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;
    Serial2.println("Drain pump OFF");
  } else {
    Serial2.println("Unknown command.");
  }
}