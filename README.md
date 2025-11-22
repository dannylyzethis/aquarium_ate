# Aquarium Automation & Monitoring System

A professional-grade Arduino-based aquarium control system with automatic freshwater/saltwater detection, IoT connectivity, and comprehensive safety features.

## Features

### Core Capabilities
- **Dual-Mode Operation**: Automatically detects and switches between freshwater and saltwater modes based on salinity readings (30-35 ppt)
- **Automated Water Changes**: Scheduled daily water changes at 2:00 AM with intelligent drain/fill cycles
- **Real-Time Monitoring**: Continuous monitoring of 11+ parameters including temperature, pH, TDS, turbidity, salinity, and water levels
- **Remote Control**: WiFi connectivity via ESP8266 with Blynk mobile app integration
- **Data Logging**: CSV logging to SD card every 60 seconds with RTC timestamps
- **Local Interface**: ILI9341 TFT touchscreen for on-site control and monitoring
- **Multi-Layer Safety**: Comprehensive fail-safe systems with leak detection, overheat protection, and pump monitoring

### Smart Features
- **Auto Salinity Detection**: System automatically determines aquarium type on startup
- **Water Quality Validation**: Pre-change validation ensures replacement water meets quality standards
- **Top-Off Automation**: Automatic evaporation compensation
- **Push Notifications**: Instant alerts for leaks, overheating, and water quality issues
- **Serial Command Interface**: Backdoor access for diagnostics and manual control

## Hardware Requirements

### Microcontrollers
- **Arduino Mega 2560** (primary controller)
- **ESP8266** (WiFi module - NodeMCU or similar)

### Sensors
| Component | Model/Type | Quantity | Pin/Interface |
|-----------|------------|----------|---------------|
| Water Temperature | DS18B20 OneWire | 3 | Pins 26, 27, 28 |
| Room Climate | DHT22 | 1 | Pin 29 |
| pH Sensor | Analog pH probe | 1 | A0 |
| TDS Sensor | Analog TDS meter | 1 | A1 |
| Turbidity Sensor | Analog turbidity sensor | 1 | A2 |
| Salinity Probes | Atlas Scientific (I2C) | 2 | I2C addr 100, 101 |
| Float Switches | Water level sensors | 4 | Pins 22-25 |
| Leak Detector | Digital leak sensor | 1 | Pin 30 |
| Current Sensors | ACS712 (30A) | 3 | A3, A4, A5 |
| Emergency Stop | Push button | 1 | Pin 31 |

### Actuators
| Component | Type | Pin |
|-----------|------|-----|
| Drain Pump | Relay (active-low) | Pin 2 |
| Fill Pump | Relay (active-low) | Pin 3 |
| Top-off Pump | Relay (active-low) | Pin 4 |
| RO/DI Solenoid | Relay (active-low) | Pin 5 |
| Alarm Buzzer | Piezo buzzer | Pin 12 |

### Display & Storage
- **ILI9341 TFT Display** (320x240, SPI) - CS=53, DC=48, RST=49
- **Resistive Touchscreen** - XP=8, YP=A1, XM=A2, YM=9
- **SD Card Module** (SPI) - CS=10
- **DS3231 RTC Module** (I2C)

### Power Requirements
- 5V DC for Arduino and logic circuits
- 12V DC or 120V AC for pumps (depending on relay board and pump selection)
- Consider UPS/battery backup for critical systems

## Software Requirements

### Arduino Libraries
```cpp
#include <Wire.h>              // I2C communication (built-in)
#include <SPI.h>               // SPI communication (built-in)
#include <OneWire.h>           // DS18B20 temperature sensors
#include <DallasTemperature.h> // DS18B20 helper library
#include <DHT.h>               // DHT22 humidity/temperature
#include <RTClib.h>            // DS3231 real-time clock
#include <SD.h>                // SD card logging
#include <Adafruit_GFX.h>      // Graphics library
#include <Adafruit_ILI9341.h>  // TFT display driver
#include <TouchScreen.h>       // Touchscreen support
#include <avr/wdt.h>           // Watchdog timer
```

### ESP8266 Libraries (for WiFi module)
```cpp
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
```

### Installation
1. Install [Arduino IDE](https://www.arduino.cc/en/software) (v1.8.x or v2.x)
2. Install required libraries via Library Manager:
   - OneWire
   - DallasTemperature
   - DHT sensor library (by Adafruit)
   - RTClib (by Adafruit)
   - Adafruit GFX Library
   - Adafruit ILI9341
   - Adafruit TouchScreen
3. For ESP8266: Add board support via Board Manager (ESP8266 Community)
4. Install [Blynk app](https://blynk.io/) on your smartphone

## Project Structure

### Modular Version (Recommended)
```
aquarium_project_top/
├── aquarium_project_top.ino  # Main entry point with setup() and loop()
├── config.h                   # Central configuration (pins, thresholds, I2C addresses)
├── sensors.cpp/.h             # Sensor reading functions
├── actuators.cpp/.h           # Pump and valve control
├── safety.cpp/.h              # Safety monitoring and fail-safe logic
├── display.cpp/.h             # TFT display management
├── logging.cpp/.h             # SD card data logging
├── blynk.cpp/.h               # WiFi/IoT communication
└── commands.cpp/.h            # Serial command interface
```

### Monolithic Version
```
aquarium_project.ino           # Single-file implementation (669 lines)
```

### WiFi Module
```
wifi_module_for_aquarium/
└── wifi_module_for_aquarium.ino  # ESP8266 Blynk bridge
```

## Configuration

### 1. WiFi and Blynk Setup

Edit `wifi_module_for_aquarium/wifi_module_for_aquarium.ino`:

```cpp
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "aquarium"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

char auth[] = "YOUR_AUTH_TOKEN";
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";
```

**Getting Blynk Credentials:**
1. Create account at [blynk.cloud](https://blynk.cloud)
2. Create new template or use existing
3. Copy Template ID and Auth Token
4. Configure virtual pins in Blynk app (see [Blynk Virtual Pin Mapping](#blynk-virtual-pin-mapping))

### 2. Hardware Pin Configuration

Edit `aquarium_project_top/config.h` to match your wiring:

```cpp
// Actuator Pins (active-low relays)
#define DRAIN_PUMP_PIN 2
#define FILL_PUMP_PIN 3
#define TOPOFF_PUMP_PIN 4
#define RODI_VALVE_PIN 5

// Sensor Pins
#define TEMP_TANK_PIN 26
#define TEMP_BARREL_PIN 27
#define TEMP_EQUIPMENT_PIN 28
#define DHT_PIN 29
#define PH_SENSOR_PIN A0
#define TDS_SENSOR_PIN A1
// ... (see config.h for complete list)
```

### 3. Safety Thresholds

Adjust safety limits in `config.h`:

```cpp
#define DRAIN_TIMEOUT 600000      // 10 minutes max
#define FILL_TIMEOUT 600000       // 10 minutes max
#define TOPOFF_TIMEOUT 300000     // 5 minutes max
#define MAX_WATER_TEMP 35.0       // Celsius
#define MIN_SALINITY 30.0         // ppt (for saltwater mode)
#define MAX_SALINITY 35.0         // ppt
#define PUMP_CURRENT_THRESHOLD 1.2  // 120% of normal
```

### 4. Salinity Mode Detection

The system auto-detects mode on startup:
- **Saltwater Mode**: Triggered if initial salinity is 30-35 ppt
- **Freshwater Mode**: Activated for salinity outside marine range

To force a specific mode, modify `aquarium_project_top.ino`:
```cpp
void initializeMode() {
  SALTWATER_MODE = 1;  // Force saltwater mode
  // Or
  SALTWATER_MODE = 0;  // Force freshwater mode
}
```

### 5. Water Change Schedule

Default: 2:00 AM daily. Modify in main loop:
```cpp
if (now.hour() == 2 && now.minute() == 0 && now.second() == 0) {
  performWaterChange();
}
```

## Uploading Firmware

### Step 1: Upload to Arduino Mega
1. Open `aquarium_project_top/aquarium_project_top.ino` in Arduino IDE
2. Select **Board**: "Arduino Mega or Mega 2560"
3. Select **Processor**: "ATmega2560"
4. Select correct **Port**
5. Click **Upload**

### Step 2: Upload to ESP8266
1. Open `wifi_module_for_aquarium/wifi_module_for_aquarium.ino`
2. Select **Board**: "NodeMCU 1.0 (ESP-12E Module)" or your ESP8266 variant
3. Select correct **Port**
4. Click **Upload**

### Step 3: Wire Serial Connection
Connect ESP8266 to Arduino Mega Serial1:
```
ESP8266 TX → Arduino Mega RX1 (Pin 19)
ESP8266 RX → Arduino Mega TX1 (Pin 18)
ESP8266 GND → Arduino Mega GND
```

## Usage

### Initial Setup
1. Insert formatted SD card into SD card module
2. Ensure RTC battery is installed (CR2032)
3. Connect all sensors and actuators according to pin configuration
4. Power up system
5. System will auto-detect salinity mode and initialize RTC if needed

### Local Control (TFT Display)
- **Main Screen**: Displays all sensor readings in real-time
- **Touch Interface**: Navigate between screens (implementation dependent on display.cpp)
- **Status Indicators**: Visual feedback for pump states and alerts

### Remote Control (Blynk App)

#### Blynk Virtual Pin Mapping
| Virtual Pin | Parameter | Unit |
|-------------|-----------|------|
| V1 | Tank Temperature | °C |
| V2 | Barrel Temperature | °C |
| V3 | pH Level | pH |
| V4 | Tank Salinity (saltwater mode) | ppt |
| V5 | Barrel Salinity (saltwater mode) | ppt |
| V7 | Room Temperature | °C |
| V8 | System Status | "OK" or "FAIL" |

**Push Notifications** are sent for:
- Leak detection
- Temperature overheat
- Pump overcurrent
- Water quality alerts
- Emergency stop activation

### Serial Command Interface

Connect to **Serial2** (pins 16/17) at **9600 baud** to send commands:

#### Query Commands
```
TEMP_TANK        # Read tank temperature
TEMP_BARREL      # Read barrel temperature
PH               # Read pH level
SALINITY_TANK    # Read tank salinity
SALINITY_BARREL  # Read barrel salinity
TDS              # Read total dissolved solids
TURBIDITY        # Read turbidity
STATUS           # Get system status
```

#### Control Commands
```
DRAIN_ON         # Start drain pump
DRAIN_OFF        # Stop drain pump
FILL_ON          # Start fill pump
FILL_OFF         # Stop fill pump
TOPOFF_ON        # Start top-off pump
TOPOFF_OFF       # Stop top-off pump
WATER_CHANGE     # Initiate manual water change
DISABLE_ALL      # Emergency stop - disable all pumps
```

#### Safety Commands
```
RESET_FAILSAFE   # Clear fail-safe mode (after resolving issue)
```

### Data Logging

Logs are written to `log.txt` on SD card every 60 seconds in CSV format:

```csv
Timestamp, TankTemp, BarrelTemp, EquipTemp, RoomTemp, Humidity, pH, TDS, Turbidity, TankSalinity, BarrelSalinity, TankHigh, TankLow, BarrelHigh, BarrelLow, LeakDetected, FailSafe
2025-11-22 14:30:00, 25.3, 24.8, 28.1, 22.5, 65, 7.2, 150, 4.5, 33.2, 33.1, 0, 0, 1, 0, 0, 0
```

## Safety Features

### Hardware Protection
1. **Watchdog Timer**: 8-second auto-reset prevents software hangs
2. **Active-Low Relays**: Pumps default to OFF on power failure
3. **Emergency Stop Button**: Physical override on Pin 31

### Software Protection
1. **Leak Detection**: Instant pump shutdown + fail-safe mode activation
2. **Temperature Monitoring**:
   - Water max: 35°C
   - Room max: 40°C
   - Equipment overheat sensor
3. **Pump Timeout Protection**: Auto-shutoff after configured runtime
4. **Current Monitoring**: Detects clogged or failing pumps (120% threshold)
5. **Water Quality Validation**: Checks replacement water before pumping
6. **Fail-Safe Mode**: Blocks all operations until manual reset

### Fail-Safe Mode Triggers
- Leak sensor activation
- Emergency stop button pressed
- Temperature overheat
- Pump overcurrent detected
- Critical sensor failures

**Recovery**: Send `RESET_FAILSAFE` command after resolving issue

## Troubleshooting

### System Won't Start
- **Check**: SD card is formatted FAT32 and properly inserted
- **Check**: RTC battery is installed (CR2032)
- **Check**: Watchdog timer allows ~8 seconds for initialization
- **View**: Serial monitor at 9600 baud for debug messages

### Blynk Not Connecting
- **Verify**: ESP8266 has correct WiFi credentials
- **Verify**: Blynk Auth Token matches your template
- **Check**: ESP8266 TX/RX connected to Mega Serial1 (pins 18/19)
- **Test**: Upload Blynk example sketch to ESP8266 standalone

### Pumps Not Running
- **Check**: Fail-safe mode is not active (send `STATUS` command)
- **Check**: Relay connections are correct (active-low: LOW=ON)
- **Verify**: Float switches are working (physical water levels)
- **Test**: Manual control via serial commands

### Salinity Reading 0.00
- **Check**: Atlas Scientific probes are in I2C mode (not UART)
- **Verify**: I2C addresses match config (100 for tank, 101 for barrel)
- **Test**: Use Arduino I2C scanner to detect probes
- **Calibrate**: Follow Atlas Scientific calibration procedure

### Temperature Reading -127°C
- **Issue**: DS18B20 not detected
- **Check**: OneWire data line has 4.7kΩ pull-up resistor
- **Verify**: Sensor address is correct (see DallasTemperature examples)
- **Test**: Run OneWire scanner sketch

### SD Card Logging Failed
- **Format**: Card must be FAT32 (32GB max recommended)
- **Check**: SD CS pin matches config (Pin 10)
- **Verify**: SD module has separate power if needed
- **Test**: Run SD card example sketch from Arduino IDE

### False Leak Alerts
- **Adjust**: Leak sensor sensitivity/threshold
- **Check**: Sensor is not touching condensation
- **Verify**: Proper pull-up/pull-down resistor configuration

## Sensor Calibration

### pH Sensor
Calibrate using standard buffer solutions (4.0, 7.0, 10.0 pH):
```cpp
// In sensors.cpp - adjust formula based on calibration
float voltage = analogRead(PH_SENSOR_PIN) * (5.0 / 1024.0);
float ph = 7.0 + ((2.5 - voltage) / 0.18);  // Adjust slope (0.18) and offset (2.5)
```

### TDS Sensor
Calibrate using 1413 µS/cm calibration solution:
```cpp
// In sensors.cpp
float voltage = analogRead(TDS_SENSOR_PIN) * (5.0 / 1024.0);
float tds = voltage * CALIBRATION_FACTOR;  // Adjust CALIBRATION_FACTOR
```

### Atlas Scientific Salinity Probes
Follow manufacturer's calibration procedure:
1. Connect probe to K 1.0 calibration solution (12,880 µS)
2. Send `Cal,12880` command via I2C
3. Verify readings stabilize

## Advanced Configuration

### Changing Log Interval
Edit `aquarium_project_top/aquarium_project_top.ino`:
```cpp
unsigned long logInterval = 60000;  // 60 seconds (change as needed)
```

### Adding Custom Alerts
Edit `safety.cpp` and `blynk.cpp`:
```cpp
if (customCondition) {
  sendAlert("Your custom alert message");
  digitalWrite(BUZZER_PIN, HIGH);
}
```

### Multi-Tank Support
Duplicate sensor modules and modify I2C addresses:
```cpp
#define TANK1_SALINITY_ADDRESS 100
#define TANK2_SALINITY_ADDRESS 102
```

## Bill of Materials (BOM)

See `BOM.xlsx` for complete parts list with pricing and supplier information.

**Estimated Total Cost**: $400-600 USD (varies by component selection)

**High-Value Components**:
- Atlas Scientific Salinity Probes: ~$200 each
- Arduino Mega 2560: ~$40
- ILI9341 TFT: ~$15
- DS18B20 sensors: ~$5 each

## License

[Specify your license here - e.g., MIT, GPL, etc.]

## Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Test thoroughly on hardware
4. Submit pull request with detailed description

## Support

For issues and questions:
- Check [Troubleshooting](#troubleshooting) section
- Review sensor datasheets for calibration
- Consult Arduino forums for library-specific issues

## Safety Disclaimer

**IMPORTANT**: This system controls water pumps and electrical equipment. Improper installation or use may result in:
- Water damage
- Electrical hazards
- Equipment failure
- Harm to aquatic life

**Recommendations**:
- Use GFCI-protected outlets
- Install leak detection and floor drains
- Test all safety features before leaving unattended
- Maintain backup power (UPS) for critical systems
- Regularly inspect all wiring and connections
- Do not rely solely on automation - perform regular manual checks

The developers assume no liability for damages resulting from use of this system.

## Acknowledgments

Built with:
- Arduino platform
- Blynk IoT platform
- Adafruit libraries
- Atlas Scientific sensor technology

---

**Project Status**: Active Development
**Last Updated**: November 2025
**Hardware Tested**: Arduino Mega 2560, ESP8266 NodeMCU
**Compatible With**: Freshwater and Saltwater aquariums
