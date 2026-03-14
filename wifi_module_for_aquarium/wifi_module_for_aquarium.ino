#include "secrets.h"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// **Blynk and Wi-Fi Credentials**
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

void setup() {
  Serial.begin(9600);  // Match baud rate with Arduino Mega (Serial1)
  Blynk.begin(auth, ssid, pass);  // Connect to Blynk server
}

void loop() {
  Blynk.run();  // Handle Blynk communication
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.startsWith("NOTIFY:")) {
      String message = command.substring(7);  // Extract message after "NOTIFY:"
      Blynk.logEvent("aquarium_alert", message);  // Send notification/event to Blynk
    }
  }
}
