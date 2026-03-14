#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Feature toggles
extern int SALTWATER_MODE;  // 0 for freshwater, 1 for saltwater (set at runtime)
#define MARINE_SALINITY_MIN 30.0  // Minimum salinity for marine (ppt)
#define MARINE_SALINITY_MAX 35.0  // Maximum salinity for marine (ppt)

// Primary board target: Arduino GIGA R1 WiFi.
// Keep logical names stable so behavior modules remain unchanged.
#if defined(ARDUINO_GIGA) || defined(ARDUINO_GIGA_R1_WIFI) || defined(ARDUINO_ARCH_MBED)
// Pin definitions (GIGA)
#define DRAIN_PUMP_PIN 2
#define FILL_PUMP_PIN 3
#define TOPOFF_PUMP_PIN 4
#define RODI_VALVE_PIN 5
#define BUZZER_PIN 12
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
#define DHT_PIN 29
#define DRAIN_CURRENT_PIN A3
#define FILL_CURRENT_PIN A4
#define TOPOFF_CURRENT_PIN A5
#define SD_CS_PIN 10
#define TFT_CS 53
#define TFT_DC 48
#define TFT_RST 49
#define YP A8
#define XM A9
#define YM 9
#define XP 8
#else
// Legacy fallback (Mega-style mapping)
#define DRAIN_PUMP_PIN 2
#define FILL_PUMP_PIN 3
#define TOPOFF_PUMP_PIN 4
#define RODI_VALVE_PIN 5
#define BUZZER_PIN 12
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
#define DHT_PIN 29
#define DRAIN_CURRENT_PIN A3
#define FILL_CURRENT_PIN A4
#define TOPOFF_CURRENT_PIN A5
#define SD_CS_PIN 10
#define TFT_CS 53
#define TFT_DC 48
#define TFT_RST 49
#define YP A8
#define XM A9
#define YM 9
#define XP 8
#endif

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
