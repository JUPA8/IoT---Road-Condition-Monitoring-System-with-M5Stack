# Technical Documentation

## System Architecture Details

### Hardware Architecture

The Road Condition Monitoring System is built around the M5Stack Core ESP32 platform, which integrates several components:

```
┌──────────────────────────────────────────────────────────────┐
│                     M5Stack Core ESP32                        │
│                                                                │
│  ┌────────────────┐         ┌─────────────────┐              │
│  │  ESP32-WROVER  │◄────────┤   LCD Display   │              │
│  │   Dual-core    │         │    320x240      │              │
│  │  240MHz CPU    │         └─────────────────┘              │
│  └────────┬───────┘                                           │
│           │                                                    │
│      ┌────┴────┐                                              │
│      │         │                                              │
│  ┌───▼───┐ ┌──▼───┐       ┌──────────────┐                  │
│  │ WiFi  │ │ BLE  │       │   MPU9250    │                  │
│  │Module │ │Module│       │   9-axis IMU │                  │
│  └───────┘ └──────┘       └──────┬───────┘                  │
│                                   │ I2C                       │
│  ┌─────────────┐                  │                          │
│  │   Buttons   │                  │                          │
│  │  A  B  C    │            ┌─────▼──────┐                  │
│  └─────────────┘            │   GPIO     │                  │
│                              │  Pins      │                  │
└─────────────────────────────┤  16,17     ├─────────────────┘
                               └─────┬──────┘
                                     │ UART
                                     │
                               ┌─────▼──────┐
                               │  NEO-M8N   │
                               │ GPS Module │
                               │            │
                               │  External  │
                               │  Antenna   │
                               └────────────┘
```

### Software Architecture

#### Main Loop Structure

```cpp
setup() {
  1. Initialize M5Stack hardware
  2. Initialize IMU sensor
  3. Setup display
  4. Initialize GPS serial communication
  5. Connect to WiFi
  6. Configure InfluxDB client
  7. Validate InfluxDB connection
}

loop() {
  1. Update button states (M5.update())
  2. Check Button A for start position setting
  3. Read and parse GPS data continuously
  4. Read IMU acceleration data
  5. Calculate acceleration magnitude
  6. Update display every 1 second:
     - GPS data (satellites, HDOP, coordinates, time)
     - Position tracking (distance, duration)
     - Acceleration data (x, y, z axes)
     - Road condition status
  7. Send data to InfluxDB every 5 seconds:
     - GPS coordinates
     - Satellite count and HDOP
     - Speed
     - Acceleration data
     - Distance from start
}
```

#### Data Flow Diagram

```
GPS Module ──UART──► Parse NMEA ──┬──► Display
                                   └──► InfluxDB

IMU Sensor ──I2C───► Read Accel ──┬──► Calculate ──► Bump Detection
                                   ├──► Display
                                   └──► InfluxDB

Button A ──────────► Set Start Position ──► Distance Calculation
```

## Sensor Deep Dive

### GPS Module (NEO-M8N)

#### NMEA Sentence Parsing

The TinyGPSPlus library parses NMEA-0183 sentences. Key sentence types:

**$GPGGA - Global Positioning System Fix Data**
```
$GPGGA,123519.00,5216.200,N,00803.300,E,1,08,0.9,545.4,M,46.9,M,,*47
   │      │        │        │    │      │ │  │   │      │     │
   │      │        │        │    │      │ │  │   │      │     └─ Checksum
   │      │        │        │    │      │ │  │   │      └─ Geoidal separation
   │      │        │        │    │      │ │  │   └─ Altitude
   │      │        │        │    │      │ │  └─ HDOP
   │      │        │        │    │      │ └─ Satellites used
   │      │        │        │    │      └─ Fix quality
   │      │        │        │    └─ Longitude
   │      │        │        └─ Latitude hemisphere
   │      │        └─ Latitude (DDMM.MMMM format)
   │      └─ UTC Time (HHMMSS.SS)
   └─ Sentence identifier
```

**Coordinate Conversion:**
- NMEA format: DDMM.MMMM (Degrees + Minutes)
- Decimal format: DD.DDDDDD (Decimal Degrees)
- Conversion: DD.DDDDDD = DD + (MM.MMMM / 60)

Example:
- NMEA: 5216.200 N = 52° 16.200' N
- Decimal: 52 + (16.200 / 60) = 52.27° N

#### GPS Fix Quality Indicators

| Value | Meaning |
|-------|---------|
| 0 | Invalid fix |
| 1 | GPS fix (SPS) |
| 2 | DGPS fix |
| 3 | PPS fix |
| 4 | Real Time Kinematic |
| 5 | Float RTK |
| 6 | Estimated (dead reckoning) |
| 7 | Manual input mode |
| 8 | Simulation mode |

### IMU Sensor (MPU9250)

#### Sensor Specifications

**Accelerometer:**
- Full-scale range: ±2g, ±4g, ±8g, ±16g
- 16-bit ADC resolution
- Sensitivity: 16384 LSB/g (±2g range)
- Output rate: up to 4000 Hz

**Axes Orientation:**
```
        M5Stack Top View
        ┌───────────────┐
        │      USB      │
        │               │
        │       ▲ Y     │
        │       │       │
        │   X◄──┼       │
        │       │       │
        │       Z (out) │
        │               │
        │  [A] [B] [C]  │
        └───────────────┘
```

#### Acceleration Magnitude Calculation

```cpp
// Read accelerometer values in g (gravity units)
float ax_g, ay_g, az_g;
M5.Imu.getAccel(&ax_g, &ay_g, &az_g);

// Calculate vector magnitude
float magnitude = sqrt(ax_g² + ay_g² + az_g²);

// At rest: magnitude ≈ 1.0g (gravity)
// In motion: magnitude varies with vehicle dynamics
```

**Physical Interpretation:**
- 1g = 9.81 m/s²
- Magnitude > 1g indicates additional acceleration forces
- Bumps create sudden spikes in vertical (Z) acceleration
- Magnitude calculation captures total acceleration regardless of direction

#### Bump Detection Algorithm

```cpp
const float SMOOTH_THRESHOLD = 1.2;  // g
const float ROUGH_THRESHOLD = 1.5;   // g

if (magnitude > ROUGH_THRESHOLD) {
    status = "BUMP DETECTED";  // Red alert
} else if (magnitude > SMOOTH_THRESHOLD) {
    status = "Rough surface";  // Yellow warning
} else {
    status = "Smooth ride";    // Green OK
}
```

**Threshold Tuning:**
- Urban roads: Lower thresholds (1.2g / 1.4g)
- Off-road: Higher thresholds (1.5g / 2.0g)
- Bicycles: More sensitive (1.1g / 1.3g)
- Vehicles: Less sensitive (1.3g / 1.6g)

## Communication Protocols

### UART Communication (GPS)

```cpp
HardwareSerial GPSSerial(2);  // Use UART2
GPSSerial.begin(9600, SERIAL_8N1, 16, 17);
// Baud: 9600
// Format: 8 data bits, No parity, 1 stop bit
// RX pin: 16, TX pin: 17
```

**Data Format:**
- Asynchronous serial communication
- Start bit: 1
- Data bits: 8 (LSB first)
- Parity: None
- Stop bit: 1
- Baud rate: 9600 bits/second

### I2C Communication (IMU)

```cpp
// Default I2C configuration for M5Stack
Wire.begin(21, 22, 400000);
// SDA: GPIO 21
// SCL: GPIO 22
// Clock: 400 kHz (Fast mode)
```

**I2C Transaction:**
1. Start condition
2. Address byte (MPU9250: 0x68 or 0x69)
3. Register address
4. Data byte(s)
5. Stop condition

### WiFi Communication

```cpp
WiFi.begin(SSID, PASSWORD);
// Connects to 2.4GHz 802.11 b/g/n networks
// ESP32 does not support 5GHz (802.11ac)
```

**Connection Sequence:**
1. Scan for networks
2. Authenticate with WPA2-PSK
3. Obtain IP via DHCP
4. Establish connection

### HTTPS Communication (InfluxDB)

```cpp
InfluxDBClient client(URL, ORG, BUCKET, TOKEN, CACert);
// Uses TLS 1.2 for secure communication
// Certificate validation via CACert
```

**Data Transmission Format:**
```
POST /api/v2/write?org=hsos&bucket=iot-bucket HTTP/1.1
Host: eu-central-1-1.aws.cloud2.influxdata.com
Authorization: Token YOUR_TOKEN
Content-Type: text/plain

measurement_name,tag1=value1 field1=1.0,field2=2.0 timestamp
```

## InfluxDB Schema Design

### Data Point Structure

```
measurement: group_Ahmed_Achraf_Bouker
├── tags (indexed):
│   ├── device: "m5stack"
│   └── location: "road_test"
├── fields (not indexed):
│   ├── latitude: 52.270333 (float)
│   ├── longitude: 8.055000 (float)
│   ├── satellites: 12 (integer)
│   ├── hdop: 0.85 (float)
│   ├── speed: 15.3 (float, km/h)
│   ├── acc_x: -0.045 (float, g)
│   ├── acc_y: 0.123 (float, g)
│   ├── acc_z: 0.985 (float, g)
│   ├── acc_magnitude: 1.02 (float, g)
│   └── distance_from_start: 453.2 (float, meters)
└── timestamp: 1700743920000000000 (nanoseconds since epoch)
```

### Query Examples

**Basic time-series query:**
```flux
from(bucket: "iot-bucket")
  |> range(start: -1h)
  |> filter(fn: (r) => r["_measurement"] == "group_Ahmed_Achraf_Bouker")
  |> filter(fn: (r) => r["_field"] == "acc_magnitude")
```

**Aggregate bump detection events:**
```flux
from(bucket: "iot-bucket")
  |> range(start: -1h)
  |> filter(fn: (r) => r["_measurement"] == "group_Ahmed_Achraf_Bouker")
  |> filter(fn: (r) => r["_field"] == "acc_magnitude")
  |> filter(fn: (r) => r["_value"] > 1.5)
  |> count()
```

**Calculate average speed:**
```flux
from(bucket: "iot-bucket")
  |> range(start: -1h)
  |> filter(fn: (r) => r["_measurement"] == "group_Ahmed_Achraf_Bouker")
  |> filter(fn: (r) => r["_field"] == "speed")
  |> mean()
```

**GPS track with bumps highlighted:**
```flux
from(bucket: "iot-bucket")
  |> range(start: -1h)
  |> filter(fn: (r) => r["_measurement"] == "group_Ahmed_Achraf_Bouker")
  |> filter(fn: (r) => r["_field"] == "latitude" or r["_field"] == "longitude" or r["_field"] == "acc_magnitude")
  |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
  |> map(fn: (r) => ({ r with bump: if r.acc_magnitude > 1.5 then 1.0 else 0.0 }))
```

## Performance Optimization

### Memory Management

**ESP32 Memory:**
- SRAM: 520 KB (split between two cores)
- Flash: 4 MB (program storage)
- PSRAM: 4 MB (external RAM on WROVER)

**Memory Usage:**
```cpp
// Check free heap
Serial.println(ESP.getFreeHeap());

// Monitor stack usage
Serial.println(uxTaskGetStackHighWaterMark(NULL));
```

### Processing Optimization

**Timing Strategy:**
- GPS parsing: Continuous (non-blocking)
- IMU reading: On-demand (1 Hz for display)
- Display update: 1 Hz (avoid flicker)
- InfluxDB write: 5 seconds (reduce network traffic)

**CPU Load Distribution:**
```cpp
Core 0: Arduino tasks, WiFi/BLE
Core 1: Loop(), sensor reading
```

### Network Optimization

**Batch Data Points:**
```cpp
// Instead of sending individual points
// Accumulate and send multiple points together
sensorData.clearFields();
sensorData.addField("field1", value1);
sensorData.addField("field2", value2);
sensorData.addField("field3", value3);
client.writePoint(sensorData);  // Single HTTP request
```

**Connection Persistence:**
- WiFi: Keep-alive enabled
- HTTPS: Connection reuse
- Reduces handshake overhead

## Power Consumption

### Power Modes

**Active Mode:**
- ESP32: ~160-260 mA
- Display: ~60 mA
- GPS: ~60 mA
- IMU: ~3 mA
- **Total: ~280-380 mA @ 5V**

**Battery Life Estimation:**
- M5Stack 150mAh battery: ~20-30 minutes
- 2000mAh power bank: 5-7 hours
- 10000mAh power bank: 26-35 hours

### Power Optimization Strategies

```cpp
// Reduce display brightness
M5.Lcd.setBrightness(128);  // 0-255

// Sleep unused peripherals
// (Not implemented in current version)
esp_wifi_set_ps(WIFI_PS_MIN_MODEM);  // WiFi power save

// Reduce GPS update rate
// (Requires GPS module configuration)
```

## Calibration Procedures

### IMU Calibration

**Factory Calibration:** MPU9250 comes pre-calibrated

**Field Calibration (if needed):**
```cpp
void calibrateIMU() {
  float ax_offset = 0, ay_offset = 0, az_offset = 0;
  const int samples = 100;
  
  for(int i = 0; i < samples; i++) {
    float ax, ay, az;
    M5.Imu.getAccel(&ax, &ay, &az);
    ax_offset += ax;
    ay_offset += ay;
    az_offset += (az - 1.0);  // Remove 1g from Z
    delay(10);
  }
  
  ax_offset /= samples;
  ay_offset /= samples;
  az_offset /= samples;
  
  // Apply offsets in readings
}
```

### GPS Warm Start

**Cold Start Optimization:**
1. Save almanac data to EEPROM
2. Provide approximate position and time
3. Reduces time to first fix (TTFF)

```cpp
// Not implemented in current version
// Requires EEPROM library and GPS configuration
```

## Error Handling

### GPS Errors

```cpp
if (!gps.location.isValid()) {
  // Handle invalid GPS data
  // Display "---" or previous valid position
}

if (gps.satellites.value() < 4) {
  // Insufficient satellites for accurate fix
  // Display warning to user
}

if (gps.hdop.hdop() > 5.0) {
  // Poor position accuracy
  // Flag data as unreliable
}
```

### Network Errors

```cpp
if (WiFi.status() != WL_CONNECTED) {
  // WiFi disconnected
  // Attempt reconnection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

if (!client.writePoint(sensorData)) {
  // InfluxDB write failed
  String error = client.getLastErrorMessage();
  Serial.println("Error: " + error);
  // Optionally: Buffer data locally and retry
}
```

### Display Errors

```cpp
// Handle display overflow
char buf[32];  // Ensure buffer is large enough
snprintf(buf, sizeof(buf), "%.6f", latitude);  // Safe string formatting
```

## Testing Procedures

### Unit Testing

**GPS Module Test:**
```cpp
void testGPS() {
  unsigned long start = millis();
  while (millis() - start < 60000) {  // 1 minute
    while (GPSSerial.available()) {
      char c = GPSSerial.read();
      Serial.write(c);  // Echo raw NMEA
      gps.encode(c);
    }
  }
  
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("HDOP: ");
  Serial.println(gps.hdop.hdop());
}
```

**IMU Test:**
```cpp
void testIMU() {
  float ax, ay, az;
  for(int i = 0; i < 100; i++) {
    M5.Imu.getAccel(&ax, &ay, &az);
    Serial.printf("%.3f, %.3f, %.3f\n", ax, ay, az);
    delay(100);
  }
}
```

**Network Test:**
```cpp
void testNetwork() {
  if (client.validateConnection()) {
    Serial.println("InfluxDB: OK");
  } else {
    Serial.println("InfluxDB: FAIL");
    Serial.println(client.getLastErrorMessage());
  }
}
```

### Integration Testing

1. **Stationary Test:**
   - Place device on stable surface
   - Verify GPS lock
   - Confirm acceleration ~1g (gravity only)
   - Check data appears in InfluxDB

2. **Motion Test:**
   - Walk with device
   - Observe distance calculation
   - Verify GPS track in InfluxDB
   - Confirm smooth ride status

3. **Bump Test:**
   - Drop device gently from 10cm
   - Should trigger bump detection
   - Verify alert on display
   - Check spike in InfluxDB data

4. **Endurance Test:**
   - Run continuously for 1 hour
   - Monitor memory usage
   - Check for WiFi disconnects
   - Verify continuous data logging

## Troubleshooting Guide

### Debug Serial Output

Enable verbose debugging:
```cpp
#define DEBUG 1

#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif
```

### Common Issues and Solutions

| Symptom | Possible Cause | Solution |
|---------|---------------|----------|
| No GPS fix | Indoor location | Move outdoors with clear sky view |
| High HDOP | Poor satellite geometry | Wait for more satellites |
| WiFi won't connect | Wrong credentials | Verify SSID/password |
| Display frozen | Loop blocking | Add delay(), check for infinite loops |
| InfluxDB errors | Network issues | Check internet, verify token |
| Erratic acceleration | Poor mounting | Secure device firmly |
| Memory errors | Heap fragmentation | Reduce string usage, use static buffers |

## Future Enhancements

### Potential Improvements

1. **Data Storage:**
   - SD card logging for offline operation
   - Buffer data during network outages
   - CSV export functionality

2. **Advanced Analytics:**
   - Machine learning bump classification
   - Road surface type detection
   - Predictive maintenance alerts

3. **Power Management:**
   - Deep sleep between measurements
   - Solar panel integration
   - Battery percentage display

4. **User Interface:**
   - Web dashboard on ESP32
   - Mobile app via Bluetooth
   - Voice alerts for critical bumps

5. **Sensor Fusion:**
   - Combine GPS + accelerometer for dead reckoning
   - Use magnetometer for heading
   - Barometer for altitude tracking

## References

### Documentation
- [ESP32 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
- [M5Stack Arduino Library](https://github.com/m5stack/M5Stack)
- [MPU9250 Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf)
- [NEO-M8N Protocol Specification](https://www.u-blox.com/sites/default/files/products/documents/u-blox8-M8_ReceiverDescrProtSpec_UBX-13003221.pdf)
- [InfluxDB Flux Language](https://docs.influxdata.com/flux/v0.x/)

### Research Papers
- Mohamed et al. (2018): "Cloud-Assisted Real-Time Road Condition Monitoring System for Vehicles"
- Gawad et al. (2016): "Dynamic Mapping of Road Conditions Using Smartphone Sensors and Machine Learning Techniques"
- Braun & Lellmann (2019): "Verfahren und Vorrichtung zum Detektieren von Anomalien in Signaldaten für eine Reibwertschätzung für ein Fahrzeug"

---

**Last Updated**: November 2024  
**Version**: 1.0  
**Author**: Abdelrahman Ahmed
