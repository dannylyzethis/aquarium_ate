#include <Arduino.h>
#include <RTClib.h>
#include "config.h"
#include "logging.h"
#include "sensors.h"
#include "actuators.h"
#include "safety.h"

File logFile;
RTC_DS3231 rtc;

const unsigned long LOG_INTERVAL = 300000;  // Define here as a variable, not a macro
unsigned long lastLogTime = 0;

void setupLogging() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD initialization failed!");
  } else {
    Serial.println("SD initialized.");
  }
}

void logData() {
  DateTime now = rtc.now();
  logFile = SD.open("log.txt", FILE_WRITE);
  if (logFile) {
    if (logFile.size() == 0) {
      logFile.println("Timestamp,Tank Temp,Barrel Temp,Equip Temp,pH,TDS,Turbidity,Tank Level,Barrel Level,Room Temp,Humidity,Drain Pump,Fill Pump,Top-Off Pump,RO/DI Valve,Fail-Safe,Salinity");
    }
    logFile.print(now.year(), DEC);
    logFile.print('/');
    logFile.print(now.month(), DEC);
    logFile.print('/');
    logFile.print(now.day(), DEC);
    logFile.print(' ');
    logFile.print(now.hour(), DEC);
    logFile.print(':');
    logFile.print(now.minute(), DEC);
    logFile.print(':');
    logFile.print(now.second(), DEC);
    logFile.print(',');
    logFile.print(tankTemp);
    logFile.print(',');
    logFile.print(barrelTemp);
    logFile.print(',');
    logFile.print(secondaryTemp);
    logFile.print(',');
    logFile.print(pH);
    logFile.print(',');
    logFile.print(tds);
    logFile.print(',');
    logFile.print(turbidity);
    logFile.print(',');
    logFile.print(tankLevel);
    logFile.print(',');
    logFile.print(barrelLevel);
    logFile.print(',');
    logFile.print(roomTemp);
    logFile.print(',');
    logFile.print(humidity);
    logFile.print(',');
    logFile.print(drainPumpState ? "ON" : "OFF");
    logFile.print(',');
    logFile.print(fillPumpState ? "ON" : "OFF");
    logFile.print(',');
    logFile.print(topOffPumpState ? "ON" : "OFF");
    logFile.print(',');
    logFile.print(rodiValveState ? "ON" : "OFF");
    logFile.print(',');
    logFile.print(failSafeMode ? "ON" : "OFF");
    logFile.print(',');
    logFile.println(SALINITY_MONITORING ? salinity : 0.0);
    logFile.close();
    Serial.println("Logged data.");
  } else {
    Serial.println("Error opening log file.");
  }
}