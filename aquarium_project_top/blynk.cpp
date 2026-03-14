#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"
#include <BlynkSimpleWifi.h>
#include "config.h"
#include "blynk.h"
#include "sensors.h"

static const unsigned long BLYNK_RECONNECT_INTERVAL_MS = 10000;
static unsigned long lastReconnectAttempt = 0;

void setupBlynk() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < 20000) {
    delay(250);
  }

  Blynk.config(BLYNK_AUTH_TOKEN);
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.connect(5000);
  }
}

void sendToBlynk() {
  if (!Blynk.connected()) {
    return;
  }

  Blynk.virtualWrite(V1, tankTemp);
  Blynk.virtualWrite(V2, barrelTemp);
  Blynk.virtualWrite(V3, pH);
  if (SALTWATER_MODE) {
    Blynk.virtualWrite(V4, tankSalinity);
    Blynk.virtualWrite(V5, barrelSalinity);
  }
}

void checkBlynkCommands() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (!Blynk.connected() && (millis() - lastReconnectAttempt) >= BLYNK_RECONNECT_INTERVAL_MS) {
    lastReconnectAttempt = millis();
    Blynk.connect(1000);
  }

  if (Blynk.connected()) {
    Blynk.run();
  }
}

void sendAlert(String message) {
  if (Blynk.connected()) {
    Blynk.logEvent("aquarium_alert", message);
  }
  Serial.println("Alert: " + message);
}
