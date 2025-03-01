#include <avr/wdt.h>
#include <Arduino.h>
#include "config.h"
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
  #if SALTWATER_MODE
    if (barrelSalinity < MIN_SALINITY || barrelSalinity > MAX_SALINITY) {
      Serial.println("Water change skipped: Barrel salinity out of range");
      sendAlert("Water change skipped: Barrel salinity out of range");
      return;
    }
  #endif

  // Optional: Initial level check
  if (tankLevel == "Low" || barrelLevel == "Low") {
    Serial.println("Water change skipped: Tank or barrel level too low");
    return;
  }

  // Draining Phase
  Serial.println("Starting drain phase");
  digitalWrite(DRAIN_PUMP_PIN, LOW);  // Turn on drain pump
  drainPumpState = true;
  unsigned long drainStartTime = millis();
  while (digitalRead(TANK_LOW_PIN) == HIGH && (millis() - drainStartTime < DRAIN_TIMEOUT)) {
    wdt_reset();  // Reset watchdog timer
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
  unsigned long fillStartTime = millis();
  while (digitalRead(BARREL_LOW_PIN) == HIGH && (millis() - fillStartTime < FILL_TIMEOUT)) {
    wdt_reset();  // Reset watchdog timer
  }
  digitalWrite(FILL_PUMP_PIN, HIGH);  // Turn off fill pump
  fillPumpState = false;
  if (digitalRead(BARREL_LOW_PIN) == HIGH) {
    // Timeout reached before barrel low level was detected
    faultMessage = "Fill timeout: Barrel did not reach low level";
    Serial.println(faultMessage);
    sendAlert(faultMessage);
  } else {
    Serial.println("Fill phase complete: Barrel reached low level");
  }
}