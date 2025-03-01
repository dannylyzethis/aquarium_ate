#include <avr/wdt.h>
#include <Arduino.h>
#include "config.h"  // No LOG_INTERVAL macro here anymore
#include "sensors.h"
#include "actuators.h"
#include "safety.h"
#include "display.h"
#include "logging.h"
#include "blynk.h"
#include "commands.h"

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  setupSensors();
  setupActuators();
  setupSafety();
  setupDisplay();
  setupLogging();
  setupBlynk();

  wdt_enable(WDTO_8S);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= updateInterval) {
    lastUpdate = currentTime;
    updateSensors();
    checkSafetyConditions(currentTime);
    updateDisplay();
    sendToBlynk();
  }

  if (currentTime - lastLogTime >= LOG_INTERVAL) {
    lastLogTime = currentTime;
    logData();
  }

  handleTouch();
  checkBlynkCommands();
  handleSerialCommands();

  wdt_reset();
}