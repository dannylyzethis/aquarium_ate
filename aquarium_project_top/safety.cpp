#include <Arduino.h>
#include "config.h"
#include "safety.h"
#include "sensors.h"
#include "actuators.h"
#include "blynk.h"

bool failSafeMode = false;
String faultMessage = "";
static unsigned long lastAlertAt = 0;
static String lastAlertMessage = "";
const unsigned long ALERT_COOLDOWN_MS = 30000;

static void alertOncePerCooldown(const String &message, unsigned long currentTime) {
  if (message != lastAlertMessage || (currentTime - lastAlertAt) >= ALERT_COOLDOWN_MS) {
    sendAlert(message);
    lastAlertMessage = message;
    lastAlertAt = currentTime;
  }
}

void setupSafety() {
  pinMode(LEAK_SENSOR_PIN, INPUT);
  pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
}

void checkSafetyConditions(unsigned long currentTime) {
  if (digitalRead(LEAK_SENSOR_PIN) == HIGH) {
    failSafeMode = true;
    faultMessage = "Leak Detected";
    disableAllPumps();
    alertOncePerCooldown("Leak Detected", currentTime);
  }
  if (tankTemp > MAX_WATER_TEMP || secondaryTemp > MAX_WATER_TEMP) {
    failSafeMode = true;
    faultMessage = "Overheat Detected";
    disableAllPumps();
    alertOncePerCooldown("Overheat Detected", currentTime);
  }
  if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
    failSafeMode = true;
    faultMessage = "Emergency Stop";
    disableAllPumps();
    alertOncePerCooldown("Emergency Stop Activated", currentTime);
  }
  if (tankSalinity > MARINE_SALINITY_MAX && SALTWATER_MODE) {
    Serial.println("Salinity too high!");
    triggerAlarm();
  }
  if (drainPumpState && (currentTime - drainStartTime > DRAIN_TIMEOUT)) {
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;
    faultMessage = "Drain Timeout";
    alertOncePerCooldown("Drain Timeout", currentTime);
  }
  // Add similar checks for fill, top-off, and RO/DI if needed
}
void triggerAlarm() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);  // Buzz for 1 second
  digitalWrite(BUZZER_PIN, LOW);
}
