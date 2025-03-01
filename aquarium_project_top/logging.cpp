#include <SD.h>
#include "config.h"
#include "logging.h"
#include "sensors.h"

File logFile;

void setupLogging() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD init failed");
  }
}

void logData() {
  logFile = SD.open("log.txt", FILE_WRITE);
  if (logFile) {
    logFile.print("Tank Temp: ");
    logFile.println(tankTemp);
    logFile.print("Barrel Temp: ");
    logFile.println(barrelTemp);
    logFile.print("pH: ");
    logFile.println(pH);
    #if SALTWATER_MODE
    logFile.print("Tank Salinity: ");
    logFile.println(tankSalinity);
    logFile.print("Barrel Salinity: ");
    logFile.println(barrelSalinity);
    #endif
    logFile.close();
  }
}