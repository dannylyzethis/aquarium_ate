#include <SD.h>
#include <Wire.h>    // For I2C communication
#include <RTClib.h>  // For RTC functions
#include "config.h"
#include "logging.h"
#include "sensors.h"


File logFile;
RTC_DS3231 rtc;  // DS3231 RTC object
void setupLogging() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD init failed");
  }
}

void logData() {
  logFile = SD.open("log.txt", FILE_WRITE);  // Open log file
  if (logFile) {
    // Get current time from RTC
    DateTime now = rtc.now();
    
    // Write timestamp in YYYY-MM-DD HH:MM:SS format
    logFile.print(now.year(), DEC);
    logFile.print('-');
    logFile.print(now.month(), DEC);
    logFile.print('-');
    logFile.print(now.day(), DEC);
    logFile.print(' ');
    logFile.print(now.hour(), DEC);
    logFile.print(':');
    logFile.print(now.minute(), DEC);
    logFile.print(':');
    logFile.print(now.second(), DEC);
    logFile.print(" - ");
    
    // Add your existing log data
    logFile.print("Tank Temp: ");
    logFile.println(tankTemp);
    logFile.print("Barrel Temp: ");
    logFile.println(barrelTemp);
    logFile.print("pH: ");
    logFile.println(pH);
    if (SALTWATER_MODE) {
      logFile.print("Tank Salinity: ");
      logFile.println(tankSalinity);
      logFile.print("Barrel Salinity: ");
      logFile.println(barrelSalinity);
    }
    
    logFile.close();  // Close the file
  } else {
    Serial.println("Error opening log file");
  }
}
