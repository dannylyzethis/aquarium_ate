#include <Arduino.h>
#include "config.h"
#include "blynk.h"
#include "sensors.h"
#include "safety.h"
#include "commands.h"

void setupBlynk() {
}

void sendToBlynk() {
  if (!failSafeMode) {
    Serial1.println("V1:" + String(tankTemp));
    Serial1.println("V2:" + String(barrelTemp));
    Serial1.println("V3:" + String(pH));
    Serial1.println("V4:" + String(tds));
    Serial1.println("V5:" + String(tankLevel));
    Serial1.println("V6:" + String(barrelLevel));
    Serial1.println("V7:" + String(roomTemp));
    Serial1.println("V8:" + String(failSafeMode ? "FAIL" : "OK"));
    #if SALINITY_MONITORING
      Serial1.println("V9:" + String(salinity));
    #endif
  }
}

void checkBlynkCommands() {
  if (Serial1.available()) {
    String cmd = Serial1.readStringUntil('\n');
    cmd.trim();
    handleSerialCommands(cmd);
  }
}

void sendAlert(String message) {
  Serial1.println("NOTIFY:" + message);
}