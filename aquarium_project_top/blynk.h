#ifndef BLYNK_H
#define BLYNK_H

#include <Arduino.h>

void setupBlynk();
void sendToBlynk();
void checkBlynkCommands();
void sendAlert(String message);

#endif