#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Feature toggles
#define SALTWATER_MODE 1  // 1 for saltwater, 0 for freshwater

// Pin definitions
#define DRAIN_PUMP_PIN 2
#define FILL_PUMP_PIN 3
#define TOPOFF_PUMP_PIN 4
#define RODI_VALVE_PIN 5
#define TANK_TEMP_PIN 26
#define BARREL_TEMP_PIN 27
#define SECONDARY_TEMP_PIN 28
#define TANK_LOW_PIN 22
#define TANK_HIGH_PIN 23
#define BARREL_LOW_PIN 24
#define BARREL_HIGH_PIN 25
#define PH_PIN A0
#define TDS_PIN A1
#define TURB_PIN A2
#define LEAK_SENSOR_PIN 30
#define EMERGENCY_STOP_PIN 31
#define BUZZER_PIN 12
#define DHT_PIN 29
#define DRAIN_CURRENT_PIN A3
#define FILL_CURRENT_PIN A4
#define TOPOFF_CURRENT_PIN A5
#define SD_CS_PIN 10
#define TFT_CS 53
#define TFT_DC 48
#define TFT_RST 49
#define YP A1
#define XM A2
#define YM 9
#define XP 8

// Salinity probe I2C addresses (used if SALTWATER_MODE is 1)
#define TANK_SALINITY_I2C_ADDRESS 100
#define BARREL_SALINITY_I2C_ADDRESS 101

// Safety thresholds
#define DRAIN_TIMEOUT 600000    // 10 minutes in milliseconds
#define FILL_TIMEOUT 600000     // 10 minutes in milliseconds
#define TOPOFF_TIMEOUT 300000
#define RODI_TIMEOUT 300000
#define MAX_WATER_TEMP 35.0
#define MIN_SALINITY 30.0
#define MAX_SALINITY 35.0
#define PUMP_CURRENT_THRESHOLD 1.2

#endif