#define BLYNK_TEMPLATE_ID "TMPL2v32vtEWS"
#define BLYNK_TEMPLATE_NAME "aquarium"
#define BLYNK_AUTH_TOKEN "REDACTED_BLYNK_TOKEN"


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// **Blynk and Wi-Fi Credentials**
char auth[] = "REDACTED_BLYNK_TOKEN";  // Replace with your Blynk auth token
char ssid[] = "your_wifi_ssid";         // Replace with your Wi-Fi SSID
char pass[] = "your_wifi_password";     // Replace with your Wi-Fi password

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
      Blynk.notify(message);  // Send notification to Blynk app
    }
  }
}