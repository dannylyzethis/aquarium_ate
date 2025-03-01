#include <avr/wdt.h>
#include <Arduino.h>
#include "config.h"
#include "actuators.h"
#include "sensors.h"

bool drainPumpState = false;
bool fillPumpState = false;
bool topOffPumpState = false;
bool rodiValveState = false;
unsigned long drainStartTime = 0;
unsigned long fillStartTime = 0;
unsigned long topOffStartTime = 0;
unsigned long rodiStartTime = 0;

void setupActuators() {
  pinMode(DRAIN_PUMP_PIN, OUTPUT);
  pinMode(FILL_PUMP_PIN, OUTPUT);
  pinMode(TOPOFF_PUMP_PIN, OUTPUT);
  pinMode(RODI_VALVE_PIN, OUTPUT);

  digitalWrite(DRAIN_PUMP_PIN, HIGH);
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
  if (tankLevel != "Low" && barrelLevel != "Low") {
    digitalWrite(DRAIN_PUMP_PIN, LOW);
    drainPumpState = true;
    drainStartTime = millis();
    unsigned long start = millis();
    while (millis() - start < 300000) {  // Drain for 5 minutes
      wdt_reset();
    }
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;

    digitalWrite(FILL_PUMP_PIN, LOW);
    fillPumpState = true;
    fillStartTime = millis();
    start = millis();
    while (millis() - start < 300000) {  // Fill for 5 minutes
      wdt_reset();
    }
    digitalWrite(FILL_PUMP_PIN, HIGH);
    fillPumpState = false;
  }
}