#include <Arduino.h>
#include "config.h"
#include "blynk.h"
#include "sensors.h"

void setupBlynk() {
  Serial1.begin(9600);  // Initialize Serial1 for ESP8266
}

void sendToBlynk() {
  Serial1.println("V1:" + String(tankTemp));
  Serial1.println("V2:" + String(barrelTemp));
  Serial1.println("V3:" + String(pH));
  if (SALTWATER_MODE) {
    Serial1.println("V4:" + String(tankSalinity));
    Serial1.println("V5:" + String(barrelSalinity));
  }
}

void checkBlynkCommands() {
  if (Serial1.available()) {
    String cmd = Serial1.readStringUntil('\n');
    cmd.trim();
    // Handle Blynk commands if needed
  }
}

void sendAlert(String message) {
  Serial1.println("NOTIFY:" + message);  // Send alert command to ESP8266
  Serial.println("Sent alert: " + message);  // Debug output
}
