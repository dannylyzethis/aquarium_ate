#include <avr/wdt.h>
#include <Arduino.h>
#include "RTClib.h"
#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "safety.h"
#include "display.h"
#include "logging.h"
#include "blynk.h"
#include "commands.h"

int SALTWATER_MODE = 1;  // Global variable: 0 = freshwater, 1 = saltwater
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000;  // Update every second
unsigned long lastLogTime = 0;
const unsigned long logInterval = 60000;   // Log every minute
extern RTC_DS3231 rtc;
void setup() {
  Serial.begin(9600);    // Debugging
  Serial1.begin(9600);   // ESP8266 (Blynk)
  Serial2.begin(9600);   // Serial commands
  SALTWATER_MODE = 1;  // Global variable: 0 = freshwater, 1 = saltwater
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // Ensure it's off initially
  
  setupSensors();
  setupActuators();
  setupSafety();
  setupDisplay();
  setupLogging();
  setupBlynk();

  initializeMode();
  // Initialize the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);  // Stop if RTC isn’t detected
  }
  // Check if RTC lost power and set time if needed
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set to compile time
  }

  wdt_enable(WDTO_8S);  // Watchdog timer (8s)
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpdate >= updateInterval) {
    lastUpdate = currentTime;
    updateSensors();
    checkSafetyConditions(currentTime);
    updateDisplay();  // Update TFT display
    sendToBlynk();    // Send data to Blynk
  }
  if (currentTime - lastLogTime >= logInterval) {
    lastLogTime = currentTime;
    logData();        // Log data to SD card
  }
  handleSerialCommands();
  checkBlynkCommands();  // Check for Blynk commands
  wdt_reset();  // Reset watchdog
}
void initializeMode() {
  float initialSalinity = readSalinity(TANK_SALINITY_I2C_ADDRESS);
  if (initialSalinity >= MARINE_SALINITY_MIN && initialSalinity <= MARINE_SALINITY_MAX) {
    SALTWATER_MODE = 1;
    Serial.println("Saltwater mode enabled");
  } else if (initialSalinity == -1.0) {
    Serial.println("Invalid salinity reading, defaulting to freshwater mode");
    SALTWATER_MODE = 0;
  } else {
    SALTWATER_MODE = 0;
    Serial.println("Freshwater mode enabled");
  }
}