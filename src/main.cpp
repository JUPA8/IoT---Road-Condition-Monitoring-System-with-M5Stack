#include <M5Unified.h>
#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// ===== WIFI CONFIGURATION =====
// ⚠️ CHANGE THESE TO YOUR WIFI! ⚠️
#define WIFI_SSID "iPhone von Achraf"
#define WIFI_PASSWORD "Hello2020"

// ===== INFLUXDB CONFIGURATION =====
#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "bCfPoZ_Ys5WvjAztMdlBUioe7e_8N9vAbCuxPEWFGVhJEEr-IfI9b8QxNxMBXRCWY7hKFvWpI4ONSmFis5zwUQ=="
#define INFLUXDB_ORG "hsos"
#define INFLUXDB_BUCKET "iot-bucket"

// ⚠️ CHANGE THIS TO YOUR UNIQUE NAME! ⚠️
#define MEASUREMENT_NAME "group_Ahmed_Achraf_Bouker"

// ===== GPS CONFIGURATION =====
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_BAUD 9600

TinyGPSPlus gps;
HardwareSerial GPSSerial(2);

// InfluxDB client
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensorData(MEASUREMENT_NAME);

unsigned long lastUpdate = 0;
unsigned long lastInfluxSend = 0;

// Start position tracking
double startLat = 0.0;
double startLon = 0.0;
unsigned long startTime = 0;
bool startPositionSet = false;

void drawSeparator(int y) {
  M5.Lcd.setCursor(5, y);
  M5.Lcd.print("--------------------------------------------");
}

void setup() {
  // Initialize M5Stack
  auto cfg = M5.config();
  M5.begin(cfg);
  
  // Initialize IMU
  M5.Imu.begin();
  
  // Setup display
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextFont(1);
  M5.Lcd.setCursor(20, 60);
  M5.Lcd.println("Road Condition Monitoring");
  M5.Lcd.setCursor(20, 80);
  M5.Lcd.println("Initializing...");
  
  // Serial for debugging
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== GPS + IMU + InfluxDB ===");
  
  // Initialize GPS
  GPSSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS initialized");
  
  // Connect to WiFi
  M5.Lcd.setCursor(20, 100);
  M5.Lcd.print("WiFi: Connecting...");
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    M5.Lcd.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    M5.Lcd.setCursor(20, 120);
    M5.Lcd.setTextColor(GREEN, BLACK);
    M5.Lcd.println("WiFi: Connected!");
    M5.Lcd.setTextColor(WHITE, BLACK);
  } else {
    Serial.println("\nWiFi Failed!");
    M5.Lcd.setCursor(20, 120);
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.println("WiFi: FAILED");
  }
  
  // Configure InfluxDB
  sensorData.addTag("device", "m5stack");
  sensorData.addTag("location", "road_test");
  
  // Check InfluxDB connection
  if (WiFi.status() == WL_CONNECTED) {
    if (client.validateConnection()) {
      Serial.println("InfluxDB: Connected");
      M5.Lcd.setCursor(20, 140);
      M5.Lcd.setTextColor(GREEN, BLACK);
      M5.Lcd.println("InfluxDB: Connected");
    } else {
      Serial.print("InfluxDB Error: ");
      Serial.println(client.getLastErrorMessage());
      M5.Lcd.setCursor(20, 140);
      M5.Lcd.setTextColor(RED, BLACK);
      M5.Lcd.println("InfluxDB: Error");
    }
  }
  
  delay(2000);
  Serial.println("\nSystem Ready!");
  Serial.println("Press Button A to set start position");
}

void loop() {
  M5.update();
  
  // Button A: Set start position
  if (M5.BtnA.wasPressed()) {
    if (gps.location.isValid()) {
      startLat = gps.location.lat();
      startLon = gps.location.lng();
      startTime = millis();
      startPositionSet = true;
      Serial.println(">>> START POSITION SET!");
    }
  }
  
  // Read GPS data
  while (GPSSerial.available()) {
    gps.encode(GPSSerial.read());
  }
  
  // Read IMU data (in g, convert to mg for display)
  float ax_g, ay_g, az_g;
  int ax = 0, ay = 0, az = 0;
  if (M5.Imu.isEnabled()) {
    M5.Imu.getAccel(&ax_g, &ay_g, &az_g);
    ax = (int)(ax_g * 1000.0f);  // Convert to mg
    ay = (int)(ay_g * 1000.0f);
    az = (int)(az_g * 1000.0f);
  }
  
  // Calculate magnitude for bump detection
  float accMag = sqrt(ax_g*ax_g + ay_g*ay_g + az_g*az_g);
  
  // Update display every 1 second
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextFont(1);
    
    int y = 5;
    char buf[32];
    
    // ----------------- TITLE -----------------
    M5.Lcd.setCursor(20, y);
    M5.Lcd.print("Versuch - Road Condition Monitoring");
    y += 10;
    
    // WiFi Status
    M5.Lcd.setCursor(5, y);
    if (WiFi.status() == WL_CONNECTED) {
      M5.Lcd.setTextColor(GREEN, BLACK);
      M5.Lcd.print("WiFi: OK");
    } else {
      M5.Lcd.setTextColor(RED, BLACK);
      M5.Lcd.print("WiFi: DISCONNECTED");
    }
    M5.Lcd.setTextColor(WHITE, BLACK);
    y += 15;
    
    // ------------- FIRST TABLE: GPS OVERVIEW -------------
    const int xSats  = 5;
    const int xHdop  = 35;
    const int xLat   = 75;
    const int xLon   = 155;
    const int xDate  = 235;
    
    // Header
    M5.Lcd.setCursor(xSats, y);
    M5.Lcd.print("Sats");
    M5.Lcd.setCursor(xHdop, y);
    M5.Lcd.print("HDOP");
    M5.Lcd.setCursor(xLat, y);
    M5.Lcd.print("Latitude");
    M5.Lcd.setCursor(xLon, y);
    M5.Lcd.print("Longitude");
    M5.Lcd.setCursor(xDate, y);
    M5.Lcd.print("Date");
    y += 10;
    
    M5.Lcd.setCursor(xLat, y);
    M5.Lcd.print("(deg)");
    M5.Lcd.setCursor(xLon, y);
    M5.Lcd.print("(deg)");
    M5.Lcd.setCursor(xDate, y);
    M5.Lcd.print("Time");
    y += 7;
    
    drawSeparator(y);
    y += 10;
    
    // Data
    uint8_t sats = gps.satellites.isValid() ? gps.satellites.value() : 0;
    float hdop   = gps.hdop.isValid() ? gps.hdop.hdop() : 0.0f;
    double lat   = gps.location.isValid() ? gps.location.lat() : 0.0;
    double lon   = gps.location.isValid() ? gps.location.lng() : 0.0;
    
    M5.Lcd.setCursor(xSats, y);
    sprintf(buf, "%2d", sats);
    M5.Lcd.print(buf);
    
    M5.Lcd.setCursor(xHdop, y);
    if (gps.hdop.isValid()) {
      sprintf(buf, "%.2f", hdop);
    } else {
      sprintf(buf, "--");
    }
    M5.Lcd.print(buf);
    
    M5.Lcd.setCursor(xLat, y);
    sprintf(buf, "%.6f", lat);
    M5.Lcd.print(buf);
    
    M5.Lcd.setCursor(xLon, y);
    sprintf(buf, "%.6f", lon);
    M5.Lcd.print(buf);
    
    M5.Lcd.setCursor(xDate, y);
    if (gps.date.isValid()) {
      sprintf(buf, "%02d/%02d/%04d", gps.date.month(), gps.date.day(), gps.date.year());
    } else {
      sprintf(buf, "--/--/----");
    }
    M5.Lcd.print(buf);
    y += 10;
    
    M5.Lcd.setCursor(xDate, y);
    if (gps.time.isValid()) {
      sprintf(buf, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    } else {
      sprintf(buf, "--:--:--");
    }
    M5.Lcd.print(buf);
    y += 15;
    
    // ------------- SECOND TABLE: POSITION + DAUER + DISTANZ -------------
    const int xLat2   = 5;
    const int xLon2   = 95;
    const int xDauer  = 185;
    const int xDist   = 235;
    
    M5.Lcd.setCursor(xLat2, y);
    M5.Lcd.print("Latitude");
    M5.Lcd.setCursor(xLon2, y);
    M5.Lcd.print("Longitude");
    M5.Lcd.setCursor(xDauer, y);
    M5.Lcd.print("Dauer");
    M5.Lcd.setCursor(xDist, y);
    M5.Lcd.print("Distanz");
    y += 10;
    
    M5.Lcd.setCursor(xLat2, y);
    M5.Lcd.print("(deg)");
    M5.Lcd.setCursor(xLon2, y);
    M5.Lcd.print("(deg)");
    M5.Lcd.setCursor(xDauer, y);
    M5.Lcd.print("[s]");
    M5.Lcd.setCursor(xDist, y);
    M5.Lcd.print("[m]");
    y += 7;
    
    drawSeparator(y);
    y += 10;
    
    M5.Lcd.setCursor(xLat2, y);
    sprintf(buf, "%.6f", lat);
    M5.Lcd.print(buf);
    
    M5.Lcd.setCursor(xLon2, y);
    sprintf(buf, "%.6f", lon);
    M5.Lcd.print(buf);
    
    // Calculate time and distance from start
    unsigned long elapsedTime = 0;
    double distance = 0.0;
    
    if (startPositionSet && gps.location.isValid()) {
      elapsedTime = (millis() - startTime) / 1000;  // seconds
      distance = TinyGPSPlus::distanceBetween(startLat, startLon, lat, lon);
    }
    
    M5.Lcd.setCursor(xDauer, y);
    sprintf(buf, "%lu", elapsedTime);
    M5.Lcd.print(buf);
    
    M5.Lcd.setCursor(xDist, y);
    sprintf(buf, "%.1f", distance);
    M5.Lcd.print(buf);
    y += 15;
    
    // ------------- THIRD TABLE: ACCELERATION -------------
    const int xAx = 5;
    const int xAy = 110;
    const int xAz = 215;
    
    M5.Lcd.setCursor(xAx, y);
    M5.Lcd.print("x-Accel");
    M5.Lcd.setCursor(xAy, y);
    M5.Lcd.print("y-Accel");
    M5.Lcd.setCursor(xAz, y);
    M5.Lcd.print("z-Accel");
    y += 10;
    
    M5.Lcd.setCursor(xAx, y);
    M5.Lcd.print("(mg)");
    M5.Lcd.setCursor(xAy, y);
    M5.Lcd.print("(mg)");
    M5.Lcd.setCursor(xAz, y);
    M5.Lcd.print("(mg)");
    y += 7;
    
    drawSeparator(y);
    y += 10;
    
    M5.Lcd.setCursor(xAx, y);
    sprintf(buf, "%d", ax);
    M5.Lcd.print(buf);
    
    M5.Lcd.setCursor(xAy, y);
    sprintf(buf, "%d", ay);
    M5.Lcd.print(buf);
    
    M5.Lcd.setCursor(xAz, y);
    sprintf(buf, "%d", az);
    M5.Lcd.print(buf);
    y += 15;
    
    // Road condition indicator
    M5.Lcd.setCursor(5, y);
    if (accMag > 1.5) {
      M5.Lcd.setTextColor(RED, BLACK);
      M5.Lcd.print("!!! BUMP DETECTED !!!");
      Serial.println(">>> BUMP DETECTED!");
    } else if (accMag > 1.2) {
      M5.Lcd.setTextColor(YELLOW, BLACK);
      M5.Lcd.print("Rough surface");
    } else {
      M5.Lcd.setTextColor(GREEN, BLACK);
      M5.Lcd.print("Smooth ride");
    }
    M5.Lcd.setTextColor(WHITE, BLACK);
    y += 10;
    
    // Button instruction
    M5.Lcd.setCursor(5, y);
    M5.Lcd.setTextColor(CYAN, BLACK);
    M5.Lcd.print("BtnA: Set Start");
    M5.Lcd.setTextColor(WHITE, BLACK);
    
    // Print to Serial
    Serial.print("GPS: ");
    Serial.print(lat, 6);
    Serial.print(", ");
    Serial.print(lon, 6);
    Serial.print(" | IMU: ");
    Serial.print(accMag, 3);
    Serial.println(" g");
  }
  
  // Send to InfluxDB every 5 seconds
  if (WiFi.status() == WL_CONNECTED && millis() - lastInfluxSend > 5000) {
    lastInfluxSend = millis();
    
    sensorData.clearFields();
    
    // Add GPS data
    if (gps.location.isValid()) {
      sensorData.addField("latitude", gps.location.lat());
      sensorData.addField("longitude", gps.location.lng());
    }
    
    if (gps.satellites.isValid()) {
      sensorData.addField("satellites", gps.satellites.value());
    }
    
    if (gps.hdop.isValid()) {
      sensorData.addField("hdop", gps.hdop.hdop());
    }
    
    if (gps.speed.isValid()) {
      sensorData.addField("speed", gps.speed.kmph());
    }
    
    // Add IMU data
    float ax_g, ay_g, az_g;
    M5.Imu.getAccel(&ax_g, &ay_g, &az_g);
    float accMag = sqrt(ax_g*ax_g + ay_g*ay_g + az_g*az_g);
    
    sensorData.addField("acc_x", ax_g);
    sensorData.addField("acc_y", ay_g);
    sensorData.addField("acc_z", az_g);
    sensorData.addField("acc_magnitude", accMag);
    
    // Add distance if start position set
    if (startPositionSet && gps.location.isValid()) {
      double distance = TinyGPSPlus::distanceBetween(
        startLat, startLon, 
        gps.location.lat(), gps.location.lng()
      );
      sensorData.addField("distance_from_start", distance);
    }
    
    // Write to InfluxDB
    if (client.writePoint(sensorData)) {
      Serial.println("✓ Data sent to InfluxDB");
    } else {
      Serial.print("✗ InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  }
}