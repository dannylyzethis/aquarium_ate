#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Wire.h>
#include "config.h"
#include "sensors.h"

OneWire oneWireTank(TANK_TEMP_PIN);
OneWire oneWireBarrel(BARREL_TEMP_PIN);
OneWire oneWireSecondary(SECONDARY_TEMP_PIN);
DallasTemperature tempTank(&oneWireTank);
DallasTemperature tempBarrel(&oneWireBarrel);
DallasTemperature tempSecondary(&oneWireSecondary);
DHT dht(DHT_PIN, DHT22);

float tankTemp = 0.0, barrelTemp = 0.0, secondaryTemp = 0.0, pH = 0.0, tds = 0.0, turbidity = 0.0, roomTemp = 0.0, humidity = 0.0, salinity = 0.0;
String tankLevel = "OK", barrelLevel = "OK";

void setupSensors() {
  tempTank.begin();
  tempBarrel.begin();
  tempSecondary.begin();
  dht.begin();
  Wire.begin();
  pinMode(TANK_LOW_PIN, INPUT_PULLUP);
  pinMode(TANK_HIGH_PIN, INPUT_PULLUP);
  pinMode(BARREL_LOW_PIN, INPUT_PULLUP);
  pinMode(BARREL_HIGH_PIN, INPUT_PULLUP);
}

void updateSensors() {
  tempTank.requestTemperatures();
  tankTemp = tempTank.getTempCByIndex(0);
  tempBarrel.requestTemperatures();
  barrelTemp = tempBarrel.getTempCByIndex(0);
  tempSecondary.requestTemperatures();
  secondaryTemp = tempSecondary.getTempCByIndex(0);

  humidity = dht.readHumidity();
  roomTemp = dht.readTemperature();
  if (isnan(humidity) || isnan(roomTemp)) {
    humidity = 0.0;
    roomTemp = 0.0;
  }

  tankLevel = digitalRead(TANK_LOW_PIN) == LOW ? "Low" : digitalRead(TANK_HIGH_PIN) == HIGH ? "Full" : "OK";
  barrelLevel = digitalRead(BARREL_LOW_PIN) == LOW ? "Low" : digitalRead(BARREL_HIGH_PIN) == HIGH ? "Full" : "OK";

  pH = analogRead(PH_PIN) / 1023.0 * 14.0;
  tds = analogRead(TDS_PIN) / 1023.0 * 5000.0;
  turbidity = analogRead(TURB_PIN) / 1023.0 * 100.0;

  #if SALINITY_MONITORING
    Wire.beginTransmission(100);
    Wire.write("R");
    Wire.endTransmission();
    delay(600);
    Wire.requestFrom(100, 7);
    String response = "";
    while (Wire.available()) {
      char c = Wire.read();
      if (c == '\0') break;
      response += c;
    }
    salinity = response.toFloat();
    if (salinity < 0 || salinity > 50) salinity = -1.0;
  #else
    salinity = 0.0;
  #endif
}