#include "config.h"
#include "commands.h"
#include "actuators.h"
#include "sensors.h"
#include "safety.h"

void handleSerialCommands() {
  if (Serial2.available()) {
    String cmd = Serial2.readStringUntil('\n');
    cmd.toUpperCase();
    
    if (cmd == "TEMP_TANK") Serial2.println(String(tankTemp, 1));
    else if (cmd == "TEMP_BARREL") Serial2.println(String(barrelTemp, 1));
    else if (cmd == "TEMP_SECONDARY") Serial2.println(String(secondaryTemp, 1));
    else if (cmd == "PH") Serial2.println(String(pH, 1));
    else if (cmd == "TDS") Serial2.println(String(tds, 1));
    else if (cmd == "TURBIDITY") Serial2.println(String(turbidity, 1));
    else if (cmd == "LEVEL_TANK") Serial2.println(tankLevel);
    else if (cmd == "LEVEL_BARREL") Serial2.println(barrelLevel);
    else if (cmd == "SALINITY_TANK") {
      if (SALTWATER_MODE) Serial2.println(String(tankSalinity, 1));
      else Serial2.println("N/A (freshwater mode)");
    }
    else if (cmd == "SALINITY_BARREL") {
      if (SALTWATER_MODE) Serial2.println(String(barrelSalinity, 1));
      else Serial2.println("N/A (freshwater mode)");
    }
    
    else if (cmd == "DRAIN_ON" && !failSafeMode) {
      digitalWrite(DRAIN_PUMP_PIN, LOW);
      drainPumpState = true;
      drainStartTime = millis();
      Serial2.println("Drain pump ON");
    }
    else if (cmd == "DRAIN_OFF") {
      digitalWrite(DRAIN_PUMP_PIN, HIGH);
      drainPumpState = false;
      Serial2.println("Drain pump OFF");
    }
    else if (cmd == "FILL_ON" && !failSafeMode) {
      digitalWrite(FILL_PUMP_PIN, LOW);
      fillPumpState = true;
      fillStartTime = millis();
      Serial2.println("Fill pump ON");
    }
    else if (cmd == "FILL_OFF") {
      digitalWrite(FILL_PUMP_PIN, HIGH);
      fillPumpState = false;
      Serial2.println("Fill pump OFF");
    }
    else if (cmd == "TOPOFF_ON" && !failSafeMode) {
      digitalWrite(TOPOFF_PUMP_PIN, LOW);
      topOffPumpState = true;
      topOffStartTime = millis();
      Serial2.println("Top-off pump ON");
    }
    else if (cmd == "TOPOFF_OFF") {
      digitalWrite(TOPOFF_PUMP_PIN, HIGH);
      topOffPumpState = false;
      Serial2.println("Top-off pump OFF");
    }
    else if (cmd == "RODI_ON" && !failSafeMode) {
      digitalWrite(RODI_VALVE_PIN, LOW);
      rodiValveState = true;
      rodiStartTime = millis();
      Serial2.println("RO/DI valve ON");
    }
    else if (cmd == "RODI_OFF") {
      digitalWrite(RODI_VALVE_PIN, HIGH);
      rodiValveState = false;
      Serial2.println("RO/DI valve OFF");
    }
    else if (cmd == "WATER_CHANGE" && !failSafeMode) {
      performWaterChange();
      Serial2.println("Water change initiated");
    }
    else if (cmd == "STATUS") {
      Serial2.println("Drain: " + String(drainPumpState ? "ON" : "OFF"));
      Serial2.println("Fill: " + String(fillPumpState ? "ON" : "OFF"));
      Serial2.println("Top-off: " + String(topOffPumpState ? "ON" : "OFF"));
      Serial2.println("RO/DI: " + String(rodiValveState ? "ON" : "OFF"));
      Serial2.println("Fail-safe: " + String(failSafeMode ? "ON" : "OFF"));
      Serial2.println("Fault: " + faultMessage);
    }
    else if (cmd == "DISABLE_ALL") {
      disableAllPumps();
      Serial2.println("All pumps/valves disabled");
    }
    else if (cmd == "RESET_FAILSAFE" && failSafeMode) {
      failSafeMode = false;
      faultMessage = "";
      Serial2.println("Fail-safe reset");
    }
    else Serial2.println("Unknown command");
  }
}
