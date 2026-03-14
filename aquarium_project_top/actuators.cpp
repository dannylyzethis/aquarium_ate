#include <Arduino.h>
#include "config.h"
#include "watchdog_compat.h"
#include "actuators.h"
#include "sensors.h"
#include "safety.h"
#include "blynk.h"  // For sendAlert

bool drainPumpState = false, fillPumpState = false, topOffPumpState = false, rodiValveState = false;
unsigned long drainStartTime = 0, fillStartTime = 0, topOffStartTime = 0, rodiStartTime = 0;

void setupActuators() {
  pinMode(DRAIN_PUMP_PIN, OUTPUT);
  pinMode(FILL_PUMP_PIN, OUTPUT);
  pinMode(TOPOFF_PUMP_PIN, OUTPUT);
  pinMode(RODI_VALVE_PIN, OUTPUT);
  digitalWrite(DRAIN_PUMP_PIN, HIGH);  // HIGH = off (active-low relays)
  digitalWrite(FILL_PUMP_PIN, HIGH);
  digitalWrite(TOPOFF_PUMP_PIN, HIGH);
  digitalWrite(RODI_VALVE_PIN, HIGH);
}

void disableAllPumps() {
  digitalWrite(DRAIN_PUMP_PIN, HIGH);
  digitalWrite(FILL_PUMP_PIN, HIGH);
  digitalWrite(TOPOFF_PUMP_PIN, HIGH);
  digitalWrite(RODI_VALVE_PIN, HIGH);
  drainPumpState = fillPumpState = topOffPumpState = rodiValveState = false;
}

void performWaterChange() {
  // Optional: Pre-checks (e.g., salinity for saltwater systems)
  if (SALTWATER_MODE) {
    if (barrelSalinity < MIN_SALINITY || barrelSalinity > MAX_SALINITY) {
      Serial.println("Water change skipped: Barrel salinity out of range");
      sendAlert("Water change skipped: Barrel salinity out of range");
      return;
    }
  }

  // Optional: Initial level check
  if (tankLevel == "Low" || barrelLevel == "Low") {
    Serial.println("Water change skipped: Tank or barrel level too low");
    return;
  }

  // Draining Phase
  Serial.println("Starting drain phase");
  digitalWrite(DRAIN_PUMP_PIN, LOW);  // Turn on drain pump
  drainPumpState = true;
  drainStartTime = millis();
  while (digitalRead(TANK_LOW_PIN) == HIGH && (millis() - drainStartTime < DRAIN_TIMEOUT)) {
    APP_WDT_RESET();
  }
  digitalWrite(DRAIN_PUMP_PIN, HIGH);  // Turn off drain pump
  drainPumpState = false;
  if (digitalRead(TANK_LOW_PIN) == HIGH) {
    // Timeout reached before low level was detected
    faultMessage = "Drain timeout: Tank did not reach low level";
    Serial.println(faultMessage);
    sendAlert(faultMessage);
  } else {
    Serial.println("Drain phase complete: Tank reached low level");
  }

  // Filling Phase
  Serial.println("Starting fill phase");
  digitalWrite(FILL_PUMP_PIN, LOW);  // Turn on fill pump
  fillPumpState = true;
  fillStartTime = millis();
  while (digitalRead(TANK_HIGH_PIN) != HIGH &&
         digitalRead(BARREL_LOW_PIN) == HIGH &&
         (millis() - fillStartTime < FILL_TIMEOUT)) {
    APP_WDT_RESET();
  }
  digitalWrite(FILL_PUMP_PIN, HIGH);  // Turn off fill pump
  fillPumpState = false;
  if (digitalRead(TANK_HIGH_PIN) == HIGH) {
    Serial.println("Fill phase complete: Tank reached high level");
  } else if (digitalRead(BARREL_LOW_PIN) == LOW) {
    faultMessage = "Fill stopped: Barrel reached low level";
    Serial.println(faultMessage);
    sendAlert(faultMessage);
  } else {
    faultMessage = "Fill timeout: Tank did not reach high level";
    Serial.println(faultMessage);
    sendAlert(faultMessage);
  }
}
