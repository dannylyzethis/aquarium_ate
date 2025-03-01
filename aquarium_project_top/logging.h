#ifndef LOGGING_H
#define LOGGING_H

#include <SD.h>

extern const unsigned long LOG_INTERVAL;  // Declare as extern variable
extern unsigned long lastLogTime;

void setupLogging();
void logData();

#endif