#include <Arduino.h>
#include "config.h"
#include "safety.h"
#include "sensors.h"
#include "actuators.h"
#include "blynk.h"

float normalDrainCurrent = 0.5;
float normalFillCurrent = 0.5;
float normalTopOffCurrent = 0.3;

bool failSafeMode = false;
String faultMessage = "";

void setupSafety() {
  pinMode(LEAK_SENSOR_PIN, INPUT);
  pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void checkSafetyConditions(unsigned long currentTime) {
  if (digitalRead(LEAK_SENSOR_PIN) == HIGH) {
    failSafeMode = true;
    faultMessage = "Leak Detected";
    disableAllPumps();
    buzzerAlert();
    sendAlert("Leak Detected");
  }

  if (tankTemp > MAX_WATER_TEMP || secondaryTemp > MAX_EQUIP_TEMP) {
    failSafeMode = true;
    faultMessage = "Overheat Detected";
    disableAllPumps();
    buzzerAlert();
    sendAlert("Overheat Detected");
  }

  if (drainPumpState && readCurrent(DRAIN_CURRENT_PIN) > normalDrainCurrent * PUMP_CURRENT_THRESHOLD) {
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;
    faultMessage = "Drain Pump Overcurrent";
    buzzerAlert();
    sendAlert("Drain Pump Overcurrent");
  }
  if (fillPumpState && readCurrent(FILL_CURRENT_PIN) > normalFillCurrent * PUMP_CURRENT_THRESHOLD) {
    digitalWrite(FILL_PUMP_PIN, HIGH);
    fillPumpState = false;
    faultMessage = "Fill Pump Overcurrent";
    buzzerAlert();
    sendAlert("Fill Pump Overcurrent");
  }
  if (topOffPumpState && readCurrent(TOPOFF_CURRENT_PIN) > normalTopOffCurrent * PUMP_CURRENT_THRESHOLD) {
    digitalWrite(TOPOFF_PUMP_PIN, HIGH);
    topOffPumpState = false;
    faultMessage = "Top-Off Pump Overcurrent";
    buzzerAlert();
    sendAlert("Top-Off Pump Overcurrent");
  }

  if (humidity > MAX_HUMIDITY || roomTemp > MAX_AIR_TEMP) {
    buzzerAlert();
    sendAlert("Unsafe Environmental Conditions");
  }

  if (pH < MIN_PH || pH > MAX_PH || tds > MAX_TDS) {
    buzzerAlert();
    sendAlert("Unsafe Water Quality");
  }

  if (SALINITY_MONITORING && (salinity < MIN_SALINITY || salinity > MAX_SALINITY)) {
    buzzerAlert();
    sendAlert("Salinity Out of Range");
  }

  if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
    failSafeMode = true;
    faultMessage = "Emergency Stop Activated";
    disableAllPumps();
    buzzerAlert();
    sendAlert("Emergency Stop Activated");
  }

  if (drainPumpState && (currentTime - drainStartTime > DRAIN_TIMEOUT)) {
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;
    faultMessage = "Drain Timeout";
    buzzerAlert();
    sendAlert("Drain Timeout");
  }
  if (fillPumpState && (currentTime - fillStartTime > FILL_TIMEOUT)) {
    digitalWrite(FILL_PUMP_PIN, HIGH);
    fillPumpState = false;
    faultMessage = "Fill Timeout";
    buzzerAlert();
    sendAlert("Fill Timeout");
  }
  if (topOffPumpState && (currentTime - topOffStartTime > TOPOFF_TIMEOUT)) {
    digitalWrite(TOPOFF_PUMP_PIN, HIGH);
    topOffPumpState = false;
    faultMessage = "Top-Off Timeout";
    buzzerAlert();
    sendAlert("Top-Off Timeout");
  }
  if (rodiValveState && (currentTime - rodiStartTime > RODI_TIMEOUT)) {
    digitalWrite(RODI_VALVE_PIN, HIGH);
    rodiValveState = false;
    faultMessage = "RO/DI Timeout";
    buzzerAlert();
    sendAlert("RO/DI Timeout");
  }
}

float readCurrent(int pin) {
  return analogRead(pin) / 1023.0 * 5.0;
}

void buzzerAlert() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
}