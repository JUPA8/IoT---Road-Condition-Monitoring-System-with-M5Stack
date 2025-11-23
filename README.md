# Road Condition Monitoring System with M5Stack

[![Platform](https://img.shields.io/badge/platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Framework](https://img.shields.io/badge/framework-Arduino-00979D.svg)](https://www.arduino.cc/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Required-orange.svg)](https://platformio.org/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![IoT](https://img.shields.io/badge/IoT-Enabled-brightgreen.svg)](https://en.wikipedia.org/wiki/Internet_of_things)
[![AI/ML](https://img.shields.io/badge/AI%2FML-Ready-purple.svg)](https://www.influxdata.com/)
[![Data Analytics](https://img.shields.io/badge/Data-Analytics-yellow.svg)](https://www.influxdata.com/)

> **Note:** This project requires PlatformIO IDE. See [Installation](#installation) section for setup instructions.

A real-time IoT system for monitoring road conditions using M5Stack Core ESP32, GPS module, and accelerometer sensors. The system detects road anomalies such as bumps and rough surfaces, tracks position data, and sends telemetry to InfluxDB for visualization and analysis.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [System Architecture](#system-architecture)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [Data Visualization](#data-visualization)
- [Project Structure](#project-structure)
- [Technical Details](#technical-details)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)
- [Author](#author)

## 🎯 Overview

A real-time IoT system for monitoring road conditions using M5Stack Core ESP32, GPS module, and accelerometer sensors. The system detects road anomalies such as bumps and rough surfaces, tracks position data, and sends telemetry to InfluxDB for visualization and analysis.

**Key Technologies:** IoT • Embedded Systems • AI/ML Ready • Data Analytics • Cloud Integration • Real-time Processing

**Research Applications:** Urban Planning • Smart Cities • Transportation Engineering • Infrastructure Monitoring

### Research Background

Road condition monitoring is a current research topic in the automotive industry aimed at increasing comfort and driving safety. Traditional measurement vehicles like Road Friction Testers are expensive. This project explores a cost-effective alternative using embedded systems and cloud-based data collection.

## ✨ Features

- **Real-time GPS tracking** with NEO-M8N GPS module
- **3-axis accelerometer data** from MPU9250 IMU sensor
- **Bump detection** based on acceleration magnitude thresholds
- **Distance tracking** from a user-defined start position
- **Live display** showing GPS coordinates, satellite count, HDOP, acceleration data
- **Cloud data logging** to InfluxDB for remote monitoring
- **WiFi connectivity** for seamless data transmission
- **Interactive controls** with button-based start position setting

### Detection Thresholds

- **Smooth ride**: Acceleration < 1.2g (Green indicator)
- **Rough surface**: Acceleration 1.2g - 1.5g (Yellow indicator)
- **Bump detected**: Acceleration > 1.5g (Red alert)

## 🛠️ Hardware Requirements

- **M5Stack Core ESP32** (with display)
- **NEO-M8N GPS Module** with external antenna
- **MPU9250 IMU** (integrated in M5Stack)
- **USB-C cable** for programming and power
- Mounting equipment for bicycle/vehicle installation

### Pin Configuration

| Component | Pin | Description |
|-----------|-----|-------------|
| GPS RX | GPIO 16 | UART receive |
| GPS TX | GPIO 17 | UART transmit |
| IMU SDA | GPIO 21 | I2C data |
| IMU SCL | GPIO 22 | I2C clock |

## 💻 Software Requirements

- **PlatformIO IDE** (VS Code extension) or **PlatformIO CLI**
- **Arduino Framework** for ESP32
- **Libraries**:
  - M5Unified v0.1.16+
  - TinyGPSPlus v1.0.3+
  - ESP8266 Influxdb v3.13.1+

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        M5Stack Core                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   GPS NEO    │  │  MPU9250 IMU │  │  WiFi Module │      │
│  │   M8N Module │  │ (3-axis acc) │  │   (ESP32)    │      │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘      │
│         │                  │                  │              │
│         └──────────────────┴──────────────────┘              │
│                            │                                 │
│                    ┌───────▼───────┐                         │
│                    │  Main Program  │                         │
│                    │   Processing   │                         │
│                    └───────┬───────┘                         │
│                            │                                 │
│              ┌─────────────┴─────────────┐                   │
│              │                           │                   │
│      ┌───────▼──────┐           ┌───────▼──────┐            │
│      │  LCD Display │           │  WiFi Tx     │            │
│      │  (Real-time) │           │  (InfluxDB)  │            │
│      └──────────────┘           └──────┬───────┘            │
└─────────────────────────────────────────│──────────────────┘
                                          │
                                          │ HTTPS
                                          ▼
                              ┌───────────────────┐
                              │   InfluxDB Cloud  │
                              │   Data Storage    │
                              │   & Visualization │
                              └───────────────────┘
```

## 📥 Installation

### Step 1: Clone the Repository

```bash
git clone https://github.com/yourusername/road-condition-monitoring.git
cd road-condition-monitoring
```

### Step 2: Install PlatformIO

#### Option A: VS Code Extension
1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Install the PlatformIO IDE extension
3. Open the project folder in VS Code

#### Option B: Command Line
```bash
pip install platformio
```

### Step 3: Install Dependencies

PlatformIO will automatically install all required libraries when you first build the project. The dependencies are defined in `platformio.ini`.

### Step 4: Hardware Setup

1. **Assemble the GPS module**:
   - Screw the GPS module onto the M5Stack Core using the provided screws
   - Connect the external GPS antenna (black cable, not gray)
   
2. **Verify connections**:
   - Ensure the GPS module is properly seated
   - Check that all connections are secure

## ⚙️ Configuration

### WiFi Settings

Edit the following lines in `src/main.cpp`:

```cpp
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"
```

### InfluxDB Configuration

The default configuration connects to the Hochschule Osnabrück InfluxDB instance:

```cpp
#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_ORG "hsos"
#define INFLUXDB_BUCKET "iot-bucket"
#define INFLUXDB_TOKEN "your_token_here"
```

### Measurement Name

**IMPORTANT**: Change the measurement name to avoid data conflicts:

```cpp
#define MEASUREMENT_NAME "group_YourName_YourPartner"
```

Use a unique identifier like your name, student ID, or group name.

### GPS Settings

Default GPS configuration (usually no changes needed):

```cpp
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_BAUD 9600
```

## 🚀 Usage

### Building and Uploading

#### Using VS Code + PlatformIO:
1. Click the PlatformIO icon in the sidebar
2. Click "Build" to compile
3. Click "Upload" to flash to M5Stack
4. Click "Serial Monitor" to view debug output

#### Using Command Line:
```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Open serial monitor
pio device monitor
```

### Operating the System

1. **Power on** the M5Stack
2. **Wait for initialization**:
   - WiFi connection
   - InfluxDB validation
   - GPS satellite lock (may take 1-2 minutes outdoors)

3. **Set start position**:
   - Press **Button A** when ready to start monitoring
   - This saves the current GPS coordinates as reference point

4. **Monitor the display**:
   - **Top section**: Satellite count, HDOP, GPS coordinates, date/time
   - **Middle section**: Current position, elapsed time, distance traveled
   - **Bottom section**: Acceleration data (x, y, z axes)
   - **Status indicator**: Road condition (Smooth/Rough/Bump)

5. **Data logging**:
   - Data is automatically sent to InfluxDB every 5 seconds
   - Watch serial monitor for confirmation messages

### Display Information

```
┌─────────────────────────────────────────────────────────┐
│ Versuch - Road Condition Monitoring                     │
│ WiFi: OK                                                │
├─────────────────────────────────────────────────────────┤
│ Sats | HDOP | Latitude  | Longitude | Date       Time  │
│  12  | 0.85 | 52.270333 | 8.055000  | 23/11/2025 14:23│
├─────────────────────────────────────────────────────────┤
│ Latitude  | Longitude | Dauer [s] | Distanz [m]       │
│ 52.270333 | 8.055000  | 124       | 453.2             │
├─────────────────────────────────────────────────────────┤
│ x-Accel   | y-Accel   | z-Accel                        │
│ (mg)      | (mg)      | (mg)                           │
│ -45       | 123       | 985                            │
│ Smooth ride                                             │
│ BtnA: Set Start                                         │
└─────────────────────────────────────────────────────────┘
```

## 📊 Data Visualization

### Accessing InfluxDB

1. Navigate to: https://eu-central-1-1.aws.cloud2.influxdata.com
2. Login with provided credentials:
   - Email: `PraktikumDummy@gmail.com`
   - Password: `FJG-GKCb!GqKzn3z#5LK8e~m`

3. Click **"Explore"** in the left sidebar

### Creating Visualizations

1. **Select time range**: Choose the time period for your data
2. **Select bucket**: `iot-bucket`
3. **Select measurement**: Your unique measurement name
4. **Select fields**: Choose which data to display:
   - `latitude`, `longitude` - GPS position
   - `acc_x`, `acc_y`, `acc_z` - Individual acceleration axes
   - `acc_magnitude` - Total acceleration
   - `distance_from_start` - Distance traveled
   - `speed` - Current speed (km/h)
   - `satellites` - Number of GPS satellites
   - `hdop` - GPS accuracy metric

5. **Choose visualization type**:
   - Line graph for time-series data
   - Map view for GPS coordinates
   - Gauge for real-time values

### Example Queries

**View acceleration magnitude over time:**
```flux
from(bucket: "iot-bucket")
  |> range(start: -1h)
  |> filter(fn: (r) => r["_measurement"] == "your_measurement_name")
  |> filter(fn: (r) => r["_field"] == "acc_magnitude")
```

**Plot GPS track on map:**
```flux
from(bucket: "iot-bucket")
  |> range(start: -1h)
  |> filter(fn: (r) => r["_measurement"] == "your_measurement_name")
  |> filter(fn: (r) => r["_field"] == "latitude" or r["_field"] == "longitude")
```

## 📁 Project Structure

```
road-condition-monitoring/
│
├── src/
│   └── main.cpp              # Main program code
│
├── include/
│   └── README                # Header files directory info
│
├── lib/
│   └── README                # Custom libraries directory
│
├── platformio.ini            # PlatformIO configuration
│
├── README.md                 # This file
├── TECHNICAL_DOCS.md         # Technical documentation
├── ASSIGNMENT.md             # Original assignment description
├── LICENSE                   # MIT License
│
└── docs/
    ├── images/               # Screenshots and diagrams
    ├── setup-guide.md        # Detailed setup instructions
    └── api-reference.md      # Code documentation
```

## 🔧 Technical Details

### Data Collection

- **GPS Update Rate**: 1 Hz (one reading per second)
- **IMU Sampling**: Continuous polling in main loop
- **InfluxDB Transmission**: Every 5 seconds
- **Display Refresh**: Every 1 second

### Sensor Specifications

#### NEO-M8N GPS Module
- **Accuracy**: 2.5m CEP (Circular Error Probable)
- **Channels**: 72 acquisition, 24 tracking
- **Update Rate**: Up to 10 Hz (configured for 1 Hz)
- **Cold Start**: 26s
- **Communication**: UART at 9600 baud

#### MPU9250 IMU
- **Accelerometer Range**: ±2g, ±4g, ±8g, ±16g (configurable)
- **Gyroscope Range**: ±250, ±500, ±1000, ±2000 °/s
- **Magnetometer**: 3-axis, ±4800 µT
- **Communication**: I2C interface

### GPS Coordinate Format

The system receives NMEA sentences from the GPS module, particularly:
- **GPGGA**: Global Positioning System Fix Data
- **GPRMC**: Recommended Minimum Navigation Information

Example GPGGA sentence:
```
$GPGGA,101010.0,5216.200,N,00803.300,E,1,12,0.85,80.0,M,45.0,M,,*50
```

Breakdown:
- `101010.0` - Time: 10:10:10 UTC
- `5216.200,N` - Latitude: 52°16.200' North = 52.27° N
- `00803.300,E` - Longitude: 8°03.300' East = 8.055° E
- `1` - GPS fix quality (1 = GPS fix)
- `12` - Number of satellites
- `0.85` - HDOP (Horizontal Dilution of Precision)
- `80.0,M` - Altitude above sea level (meters)

**Location**: Near Osnabrück, Lower Saxony, Germany

### HDOP (Horizontal Dilution of Precision)

HDOP indicates GPS accuracy:
- **< 1**: Ideal (Differential GPS)
- **1-2**: Excellent
- **2-5**: Good
- **5-10**: Moderate
- **10-20**: Fair
- **> 20**: Poor

High HDOP values result from:
- Poor satellite geometry (satellites clustered together)
- Few visible satellites
- Signal obstructions (buildings, trees, mountains)
- Atmospheric conditions

### InfluxDB Data Schema

**Database Type**: Time-series database optimized for IoT data

**Key Concepts**:
- **Bucket**: Logical container for time-series data (like a database)
- **Measurement**: Similar to a table, groups related data points
- **Tags**: Indexed metadata (device, location)
- **Fields**: Actual data values (latitude, acceleration, etc.)
- **Timestamp**: Automatic time-based indexing

## 🐛 Troubleshooting

### GPS Issues

**Problem**: No GPS fix (0 satellites)
- **Solution**: 
  - Ensure external antenna is connected (black cable)
  - Move to clear outdoor location with sky view
  - Wait 1-2 minutes for cold start
  - Check pin connections (RX=16, TX=17)

**Problem**: High HDOP value (> 5)
- **Solution**:
  - Move away from tall buildings
  - Wait for more satellites to lock
  - Check for antenna damage

### WiFi Connection

**Problem**: WiFi connection fails
- **Solution**:
  - Verify SSID and password
  - Check router is 2.4GHz (ESP32 doesn't support 5GHz)
  - Move closer to router
  - Check serial monitor for error messages

### InfluxDB

**Problem**: Data not appearing in InfluxDB
- **Solution**:
  - Verify WiFi is connected
  - Check measurement name is unique
  - Confirm InfluxDB token is correct
  - Look for error messages in serial monitor
  - Verify time range in InfluxDB query

**Problem**: "InfluxDB write failed" error
- **Solution**:
  - Check internet connectivity
  - Verify cloud service is accessible
  - Ensure token has write permissions

### Display Issues

**Problem**: Display shows garbled text
- **Solution**:
  - Reset M5Stack (power cycle)
  - Re-upload firmware
  - Check M5Unified library version

### Acceleration Readings

**Problem**: Constant bump detection
- **Solution**:
  - Adjust threshold values in code
  - Calibrate IMU (hold device still during startup)
  - Check mounting is secure

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/improvement`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature/improvement`)
5. Create a Pull Request

### Coding Standards

- Follow Arduino coding style
- Comment complex logic
- Update documentation for new features
- Test on hardware before submitting PR

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Author

**Abdelrahman Ahmed (Moo)**
- 🎓 Master's Student in Computer Science at Hochschule Osnabrück
- 💼 AI Developer at DroidRun-Bonny.Network GmbH
- 🔬 Specialization: IoT Systems, AI/ML, Data Analytics, Parallel Computing
- 📧 Email: [Your Email]
- 🐙 GitHub: [@JUPA8](https://github.com/JUPA8)
- 🌐 Location: Osnabrück, Germany

### Acknowledgments

- **Hochschule Osnabrück** - IoT/Industry 4.0 Laboratory
- **Supervisors**: 
  - Marco Schaarschmidt (m.schaarschmidt@hs-osnabrueck.de)
  - Nicolas Lampe (n.lampe@hs-osnabrueck.de)
  - Tim Seidel (t.seidel@hs-osnabrueck.de)
  - Simon Balzer (simon.balzer@hs-osnabrueck.de)
- **Project Partner**: Achraf Bouker

### References

- [M5Stack Documentation](https://docs.m5stack.com/)
- [TinyGPS++ Library](http://arduiniana.org/libraries/tinygpsplus/)
- [InfluxDB Documentation](https://docs.influxdata.com/)
- Research papers on road condition monitoring (see ASSIGNMENT.md)

---

**Course**: Internet of Things (IoT) / Industry 4.0  
**Institution**: Hochschule Osnabrück, Germany  
**Semester**: Winter 2024/2025  
**Version**: 2.2

---

⭐ If you find this project helpful, please consider giving it a star!
