#include <avr/wdt.h>  // Watchdog timer library
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <SD.h>  // SD library for data logging

// **Toggle Salinity Monitoring (1 = enabled, 0 = disabled)**
#define SALINITY_MONITORING 1

// **TFT Pins**
#define TFT_CS 53
#define TFT_DC 48
#define TFT_RST 49

// **Touch Pins**
#define YP A1
#define XM A2
#define YM 9
#define XP 8

// **Sensor Pins**
#define TANK_TEMP_PIN 26
#define BARREL_TEMP_PIN 27
#define SECONDARY_TEMP_PIN 28  // Secondary temp sensor for overheat protection
#define TANK_LOW_PIN 22
#define TANK_HIGH_PIN 23
#define BARREL_LOW_PIN 24
#define BARREL_HIGH_PIN 25
#define PH_PIN A0
#define TDS_PIN A1
#define TURB_PIN A2
#define LEAK_SENSOR_PIN 30     // Leak detection sensor
#define DHT_PIN 29             // DHT22 for environmental monitoring
#define EMERGENCY_STOP_PIN 31  // Emergency stop button
#define BUZZER_PIN 12          // Buzzer for alerts

// **Current Sensor Pins**
#define DRAIN_CURRENT_PIN A3
#define FILL_CURRENT_PIN A4
#define TOPOFF_CURRENT_PIN A5

// **Relay Pins (Active Low)**
#define DRAIN_PUMP_PIN 2
#define FILL_PUMP_PIN 3
#define TOPOFF_PUMP_PIN 4
#define RODI_VALVE_PIN 5

// **SD Card Chip Select Pin**
#define SD_CS_PIN 10  // Changed to avoid conflict with TFT_CS (53)

// **Safety Timeouts (in milliseconds)**
#define DRAIN_TIMEOUT 600000  // 10 minutes
#define FILL_TIMEOUT 600000   // 10 minutes
#define TOPOFF_TIMEOUT 300000 // 5 minutes
#define RODI_TIMEOUT 300000   // 5 minutes

// **Logging Interval (5 minutes = 300,000 milliseconds)**
const unsigned long LOG_INTERVAL = 300000;

// **Safety Thresholds**
#define MAX_WATER_TEMP 35.0    // °C
#define MAX_EQUIP_TEMP 50.0    // °C
#define MAX_HUMIDITY 85.0      // %
#define MAX_AIR_TEMP 40.0      // °C
#define MIN_PH 6.0
#define MAX_PH 8.5
#define MAX_TDS 500.0          // ppm
#define MIN_SALINITY 30.0      // ppt
#define MAX_SALINITY 35.0      // ppt
#define PUMP_CURRENT_THRESHOLD 1.2  // 120% of normal current

// **Initialize DHT22 Sensor**
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// **Initialize Other Objects**
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

OneWire oneWireTank(TANK_TEMP_PIN);
OneWire oneWireBarrel(BARREL_TEMP_PIN);
OneWire oneWireSecondary(SECONDARY_TEMP_PIN);
DallasTemperature tempTank(&oneWireTank);
DallasTemperature tempBarrel(&oneWireBarrel);
DallasTemperature tempSecondary(&oneWireSecondary);

RTC_DS3231 rtc;

// **Global Variables**
float tankTemp = 0.0;
float barrelTemp = 0.0;
float secondaryTemp = 0.0;  // Secondary temp for overheat protection
float pH = 0.0;
float tds = 0.0;
float turbidity = 0.0;
String tankLevel = "OK";
String barrelLevel = "OK";
float roomTemp = 0.0;       // Room temperature from DHT22
float humidity = 0.0;       // Room humidity from DHT22
float salinity = 0.0;       // Salinity from probe

int currentScreen = 0; // 0: home, 1: control

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000; // Update every second

bool drainPumpState = false;
bool fillPumpState = false;
bool topOffPumpState = false;
bool rodiValveState = false;

// **Safety Variables**
bool failSafeMode = false;
unsigned long drainStartTime = 0;
unsigned long fillStartTime = 0;
unsigned long topOffStartTime = 0;
unsigned long rodiStartTime = 0;
String faultMessage = "";
float normalDrainCurrent = 0.5;  // Example normal current in amps
float normalFillCurrent = 0.5;
float normalTopOffCurrent = 0.3;

// **Data Logging Variables**
File logFile;
unsigned long lastLogTime = 0;

// **Salinity Probe I2C Address**
#define SALINITY_I2C_ADDRESS 100  // Default address for Atlas Scientific Salinity Probe

void setup() {
  // **Initialize Serial Communication**
  Serial.begin(9600);    // Debugging
  Serial1.begin(9600);   // ESP8266 (Blynk)
  Serial2.begin(9600);   // Backdoor serial (pins 16 TX2, 17 RX2)

  // **Initialize TFT**
  tft.begin();
  tft.setRotation(1);

  // **Initialize Sensors**
  tempTank.begin();
  tempBarrel.begin();
  tempSecondary.begin();
  dht.begin();
  if (!rtc.begin()) {
    Serial.println("RTC failed");
    while (1);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  Wire.begin();  // Start I2C for salinity probe

  // **Set Pin Modes**
  pinMode(TANK_LOW_PIN, INPUT_PULLUP);
  pinMode(TANK_HIGH_PIN, INPUT_PULLUP);
  pinMode(BARREL_LOW_PIN, INPUT_PULLUP);
  pinMode(BARREL_HIGH_PIN, INPUT_PULLUP);
  pinMode(LEAK_SENSOR_PIN, INPUT);
  pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
  pinMode(DRAIN_PUMP_PIN, OUTPUT);
  pinMode(FILL_PUMP_PIN, OUTPUT);
  pinMode(TOPOFF_PUMP_PIN, OUTPUT);
  pinMode(RODI_VALVE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // **Set Initial Relay States (Active Low)**
  digitalWrite(DRAIN_PUMP_PIN, HIGH);
  digitalWrite(FILL_PUMP_PIN, HIGH);
  digitalWrite(TOPOFF_PUMP_PIN, HIGH);
  digitalWrite(RODI_VALVE_PIN, HIGH);

  // **Initialize SD Card for Data Logging**
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD initialization failed!");
  } else {
    Serial.println("SD initialized.");
  }

  // **Initialize Salinity Probe (if enabled)**
  if (SALINITY_MONITORING) {
    // No specific probe-specific initialization needed beyond Wire.begin()
    // Calibration should be done manually per Atlas Scientific instructions
  }

  // **Draw Initial Screen**
  drawHomeScreen();

  // **Enable Watchdog Timer with 8-second timeout**
  wdt_enable(WDTO_8S);
}

void loop() {
  unsigned long currentTime = millis();

  // **Update Sensors and Display**
  if (currentTime - lastUpdate >= updateInterval) {
    lastUpdate = currentTime;
    updateSensors();
    checkSafetyConditions(currentTime);
    updateDisplay();
    sendToBlynk();
  }

  // **Log Data Every 5 Minutes**
  if (currentTime - lastLogTime >= LOG_INTERVAL) {
    lastLogTime = currentTime;
    logData();
  }

  // **Handle Touch Inputs**
  handleTouch();

  // **Check Blynk Commands**
  checkBlynkCommands();

  // **Perform Scheduled Tasks**
  if (!failSafeMode) {
    checkScheduledTasks();
  }

  // **Backdoor Serial Commands**
  if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    command.trim();
    handleCommand(command);
  }

  // **Reset the Watchdog Timer**
  wdt_reset();
}

// **Sensor Reading Functions**
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
    faultMessage = "DHT22 Error";
  }

  if (digitalRead(TANK_LOW_PIN) == LOW) tankLevel = "Low";
  else if (digitalRead(TANK_HIGH_PIN) == HIGH) tankLevel = "Full";
  else tankLevel = "OK";

  if (digitalRead(BARREL_LOW_PIN) == LOW) barrelLevel = "Low";
  else if (digitalRead(BARREL_HIGH_PIN) == HIGH) barrelLevel = "Full";
  else barrelLevel = "OK";

  pH = readPH();
  tds = readTDS();
  turbidity = readTurbidity();

  if (SALINITY_MONITORING) {
    salinity = readSalinity();
  }
}

float readPH() {
  return analogRead(PH_PIN) / 1023.0 * 14.0; // Placeholder
}

float readTDS() {
  return analogRead(TDS_PIN) / 1023.0 * 5000.0; // Placeholder
}

float readTurbidity() {
  return analogRead(TURB_PIN) / 1023.0 * 100.0; // Placeholder
}

float readSalinity() {
  if (!SALINITY_MONITORING) return 0.0;  // Return 0 if disabled

  Wire.beginTransmission(SALINITY_I2C_ADDRESS);
  Wire.write("R");  // Command to read salinity
  Wire.endTransmission();

  delay(600);  // Wait for measurement (Atlas Scientific recommends ~600ms)
  Wire.requestFrom(SALINITY_I2C_ADDRESS, 7);  // Request 7 bytes (typical response)
  String response = "";
  while (Wire.available()) {
    char c = Wire.read();
    if (c == '\0') break;  // End of response
    response += c;
  }

  float salinityValue = response.toFloat();
  if (salinityValue < 0 || salinityValue > 50) return -1.0; // Error check
  return salinityValue;
}

// **Data Logging Function**
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

// **Safety Condition Checks with Alerts**
void checkSafetyConditions(unsigned long currentTime) {
  if (digitalRead(LEAK_SENSOR_PIN) == HIGH) {
    failSafeMode = true;
    faultMessage = "Leak Detected";
    disableAllPumps();
    buzzerAlert();
    sendAlert("Leak Detected");
  }

  if (tankTemp > MAX_WATER_TEMP || secondaryTemp > MAX_EQUIP_TEMP) {
    failSafeMode = true;
    faultMessage = "Overheat Detected";
    disableAllPumps();
    buzzerAlert();
    sendAlert("Overheat Detected");
  }

  if (drainPumpState && readCurrent(DRAIN_CURRENT_PIN) > normalDrainCurrent * PUMP_CURRENT_THRESHOLD) {
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;
    faultMessage = "Drain Pump Overcurrent";
    buzzerAlert();
    sendAlert("Drain Pump Overcurrent");
  }
  if (fillPumpState && readCurrent(FILL_CURRENT_PIN) > normalFillCurrent * PUMP_CURRENT_THRESHOLD) {
    digitalWrite(FILL_PUMP_PIN, HIGH);
    fillPumpState = false;
    faultMessage = "Fill Pump Overcurrent";
    buzzerAlert();
    sendAlert("Fill Pump Overcurrent");
  }
  if (topOffPumpState && readCurrent(TOPOFF_CURRENT_PIN) > normalTopOffCurrent * PUMP_CURRENT_THRESHOLD) {
    digitalWrite(TOPOFF_PUMP_PIN, HIGH);
    topOffPumpState = false;
    faultMessage = "Top-Off Pump Overcurrent";
    buzzerAlert();
    sendAlert("Top-Off Pump Overcurrent");
  }

  if (humidity > MAX_HUMIDITY || roomTemp > MAX_AIR_TEMP) {
    buzzerAlert();
    sendAlert("Unsafe Environmental Conditions");
  }

  if (pH < MIN_PH || pH > MAX_PH || tds > MAX_TDS) {
    buzzerAlert();
    sendAlert("Unsafe Water Quality");
  }

  if (SALINITY_MONITORING && (salinity < MIN_SALINITY || salinity > MAX_SALINITY)) {
    buzzerAlert();
    sendAlert("Salinity Out of Range");
  }

  if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
    failSafeMode = true;
    faultMessage = "Emergency Stop Activated";
    disableAllPumps();
    buzzerAlert();
    sendAlert("Emergency Stop Activated");
  }

  if (drainPumpState && (currentTime - drainStartTime > DRAIN_TIMEOUT)) {
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;
    faultMessage = "Drain Timeout";
    buzzerAlert();
    sendAlert("Drain Timeout");
  }
  if (fillPumpState && (currentTime - fillStartTime > FILL_TIMEOUT)) {
    digitalWrite(FILL_PUMP_PIN, HIGH);
    fillPumpState = false;
    faultMessage = "Fill Timeout";
    buzzerAlert();
    sendAlert("Fill Timeout");
  }
  if (topOffPumpState && (currentTime - topOffStartTime > TOPOFF_TIMEOUT)) {
    digitalWrite(TOPOFF_PUMP_PIN, HIGH);
    topOffPumpState = false;
    faultMessage = "Top-Off Timeout";
    buzzerAlert();
    sendAlert("Top-Off Timeout");
  }
  if (rodiValveState && (currentTime - rodiStartTime > RODI_TIMEOUT)) {
    digitalWrite(RODI_VALVE_PIN, HIGH);
    rodiValveState = false;
    faultMessage = "RO/DI Timeout";
    buzzerAlert();
    sendAlert("RO/DI Timeout");
  }
}

float readCurrent(int pin) {
  return analogRead(pin) / 1023.0 * 5.0;  // Placeholder conversion
}

void disableAllPumps() {
  digitalWrite(DRAIN_PUMP_PIN, HIGH);
  digitalWrite(FILL_PUMP_PIN, HIGH);
  digitalWrite(TOPOFF_PUMP_PIN, HIGH);
  digitalWrite(RODI_VALVE_PIN, HIGH);
  drainPumpState = fillPumpState = topOffPumpState = rodiValveState = false;
}

// **Buzzer Alert Function**
void buzzerAlert() {
  digitalWrite(BUZZER_PIN, HIGH);  // Turn buzzer on
  delay(1000);                     // Sound for 1 second
  digitalWrite(BUZZER_PIN, LOW);   // Turn buzzer off
}

// **Send Alert to Blynk (via ESP8266)**
void sendAlert(String message) {
  Serial1.println("NOTIFY:" + message);  // Send notification command to ESP8266
}

// **Display Functions**
void updateDisplay() {
  if (currentScreen == 0) {
    drawHomeScreen();
  } else if (currentScreen == 1) {
    drawControlScreen();
  }
}

void drawHomeScreen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);

  tft.setCursor(10, 10);
  if (failSafeMode) {
    tft.setTextColor(ILI9341_RED);
    tft.println("FAILSAFE: " + faultMessage);
  } else {
    tft.println("Aquarium Status");
  }
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(10, 40);
  tft.print("Tank Temp: ");
  tft.print(tankTemp, 1);
  tft.println(" C");

  tft.setCursor(10, 60);
  tft.print("Barrel Temp: ");
  tft.print(barrelTemp, 1);
  tft.println(" C");

  tft.setCursor(10, 80);
  tft.print("Equip Temp: ");
  tft.print(secondaryTemp, 1);
  tft.println(" C");

  tft.setCursor(10, 100);
  tft.print("Tank Level: ");
  tft.println(tankLevel);

  tft.setCursor(10, 120);
  tft.print("Barrel Level: ");
  tft.println(barrelLevel);

  tft.setCursor(10, 140);
  tft.print("pH: ");
  tft.print(pH, 1);

  tft.setCursor(10, 160);
  tft.print("TDS: ");
  tft.print(tds, 0);
  tft.println(" ppm");

  tft.setCursor(10, 180);
  tft.print("Room Temp: ");
  tft.print(roomTemp, 1);
  tft.println(" C");

  tft.setCursor(10, 200);
  tft.print("Humidity: ");
  tft.print(humidity, 1);
  tft.println(" %");

  if (SALINITY_MONITORING) {
    tft.setCursor(10, 220);
    tft.print("Salinity: ");
    tft.print(salinity, 1);
    tft.println(" ppt");
  }
}

void drawControlScreen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10, 10);
  tft.println("Control Screen");
}

// **Touch Handling**
void handleTouch() {
  if (failSafeMode) return;
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > ts.pressureThreshhold) {
    int16_t x = map(p.y, 240, 0, 0, 320);
    int16_t y = map(p.x, 0, 320, 0, 240);

    if (currentScreen == 0 && x > 280 && y < 40) {
      currentScreen = 1;
      drawControlScreen();
    } else if (currentScreen == 1 && x > 280 && y < 40) {
      currentScreen = 0;
      drawHomeScreen();
    }
  }
}

// **Blynk Integration**
void sendToBlynk() {
  if (!failSafeMode) {
    Serial1.println("V1:" + String(tankTemp));
    Serial1.println("V2:" + String(barrelTemp));
    Serial1.println("V3:" + String(pH));
    Serial1.println("V4:" + String(tds));
    Serial1.println("V5:" + String(tankLevel));
    Serial1.println("V6:" + String(barrelLevel));
    Serial1.println("V7:" + String(roomTemp));
    Serial1.println("V8:" + String(failSafeMode ? "FAIL" : "OK"));
    if (SALINITY_MONITORING) {
      Serial1.println("V9:" + String(salinity));
    }
  }
}

void checkBlynkCommands() {
  if (Serial1.available()) {
    String cmd = Serial1.readStringUntil('\n');
    cmd.trim();
    handleCommand(cmd);
  }
}

// **Scheduled Tasks**
void checkScheduledTasks() {
  if (failSafeMode) return;
  DateTime now = rtc.now();
  if (now.hour() == 2 && now.minute() == 0 && now.second() == 0) {
    performWaterChange();
  }
}

void performWaterChange() {
  if (tankLevel != "Low" && barrelLevel != "Low") {
    digitalWrite(DRAIN_PUMP_PIN, LOW);
    drainPumpState = true;
    drainStartTime = millis();
    unsigned long start = millis();
    while (millis() - start < 300000) {  // Drain for 5 minutes
      wdt_reset();
    }
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;

    digitalWrite(FILL_PUMP_PIN, LOW);
    fillPumpState = true;
    fillStartTime = millis();
    start = millis();
    while (millis() - start < 300000) {  // Fill for 5 minutes
      wdt_reset();
    }
    digitalWrite(FILL_PUMP_PIN, HIGH);
    fillPumpState = false;
  }
}

// **Backdoor Serial Communication**
void handleCommand(String cmd) {
  cmd.toUpperCase();
  if (cmd == "TEMP_TANK") {
    Serial2.println(String(tankTemp, 1));
  } else if (cmd == "TEMP_BARREL") {
    Serial2.println(String(barrelTemp, 1));
  } else if (cmd == "SALINITY" && SALINITY_MONITORING) {
    Serial2.println(String(salinity, 1));
  } else if (cmd == "STATUS") {
    Serial2.println("Drain Pump: " + String(drainPumpState ? "ON" : "OFF"));
    Serial2.println("Fill Pump: " + String(fillPumpState ? "ON" : "OFF"));
    Serial2.println("Top-Off Pump: " + String(topOffPumpState ? "ON" : "OFF"));
    Serial2.println("RO/DI Valve: " + String(rodiValveState ? "ON" : "OFF"));
    Serial2.println("Fail-Safe: " + String(failSafeMode ? "ON" : "OFF"));
  } else if (cmd == "RESET_FAILSAFE" && failSafeMode) {
    failSafeMode = false;
    faultMessage = "";
    Serial2.println("Fail-safe mode reset");
  } else if (cmd == "DRAIN_ON" && !failSafeMode) {
    digitalWrite(DRAIN_PUMP_PIN, LOW);
    drainPumpState = true;
    drainStartTime = millis();
    Serial2.println("Drain pump ON");
  } else if (cmd == "DRAIN_OFF") {
    digitalWrite(DRAIN_PUMP_PIN, HIGH);
    drainPumpState = false;
    Serial2.println("Drain pump OFF");
  } else {
    Serial2.println("Unknown command.");
  }
}