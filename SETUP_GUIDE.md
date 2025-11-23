# Setup Guide - Road Condition Monitoring System

This comprehensive guide will walk you through setting up the Road Condition Monitoring System from scratch.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Hardware Assembly](#hardware-assembly)
3. [Software Installation](#software-installation)
4. [Project Setup](#project-setup)
5. [Configuration](#configuration)
6. [First Run](#first-run)
7. [Verification](#verification)

---

## Prerequisites

### What You Need

#### Hardware
- [ ] M5Stack Core ESP32 module
- [ ] NEO-M8N GPS module with external antenna
- [ ] USB-C cable
- [ ] Computer (Windows, macOS, or Linux)
- [ ] Allen key (usually included with M5Stack)

#### Software
- [ ] Windows 10/11, macOS 10.15+, or Linux (Ubuntu 20.04+)
- [ ] Visual Studio Code
- [ ] Git (optional but recommended)
- [ ] WiFi network credentials
- [ ] InfluxDB account access

#### Knowledge Prerequisites
- Basic understanding of Arduino/C++ programming
- Familiarity with command line interface
- Basic understanding of IoT concepts

---

## Hardware Assembly

### Step 1: Prepare M5Stack Core

1. **Unbox the M5Stack Core**
   - Remove all protective films
   - Inspect for any damage

2. **Charge the battery** (if needed)
   - Connect USB-C cable
   - Charge for at least 30 minutes
   - LED indicator will show charging status

### Step 2: Attach GPS Module

1. **Position the M5Stack Core**
   - Place it display-side down on a soft surface
   - Locate the mounting holes

2. **Align the GPS module**
   - The NEO-M8N module has corresponding screw holes
   - Align carefully to avoid pin damage

3. **Secure with screws**
   - Use the Allen key to tighten screws
   - Don't overtighten - just until snug
   - Ensure module sits flush

### Step 3: Connect GPS Antenna

**IMPORTANT**: Use the correct antenna connector!

1. **Locate the antenna connectors** on the GPS module
   - **Black connector**: External antenna (use this one!)
   - **Gray connector**: Internal antenna (not used)

2. **Connect the external antenna**
   - Attach the black cable to the GPS module
   - Ensure it clicks into place
   - The antenna has a magnetic base for mounting

3. **Position the antenna**
   - For best results, place antenna with clear sky view
   - Can be mounted on a metal surface using magnetic base
   - Keep cable away from moving parts

### Step 4: Verify Hardware

**Visual Inspection Checklist:**
- [ ] GPS module firmly attached
- [ ] Screws tight but not overtightened
- [ ] Antenna cable connected to black port
- [ ] No bent pins or loose connections
- [ ] USB port accessible
- [ ] Buttons (A, B, C) are unobstructed

---

## Software Installation

### Step 1: Install Visual Studio Code

#### Windows
```powershell
# Download from official website
https://code.visualstudio.com/download

# Or use winget
winget install Microsoft.VisualStudioCode
```

#### macOS
```bash
# Download from official website
https://code.visualstudio.com/download

# Or use Homebrew
brew install --cask visual-studio-code
```

#### Linux (Ubuntu/Debian)
```bash
# Using snap
sudo snap install code --classic

# Or using apt
wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg
sudo install -o root -g root -m 644 packages.microsoft.gpg /etc/apt/trusted.gpg.d/
sudo sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/vscode stable main" > /etc/apt/sources.list.d/vscode.list'
sudo apt update
sudo apt install code
```

### Step 2: Install PlatformIO Extension

1. **Open VS Code**

2. **Open Extensions panel**
   - Click the Extensions icon in the left sidebar
   - Or press `Ctrl+Shift+X` (Windows/Linux) or `Cmd+Shift+X` (macOS)

3. **Search for PlatformIO**
   - Type "PlatformIO IDE" in the search box
   - Look for the official extension by PlatformIO

4. **Install the extension**
   - Click "Install"
   - Wait for installation (may take a few minutes)
   - Reload VS Code when prompted

5. **Verify installation**
   - You should see a new PlatformIO icon (alien head) in the left sidebar
   - Click it to open the PlatformIO home page

### Step 3: Install USB Drivers (Windows Only)

The M5Stack uses the CP2104 USB-to-UART bridge.

1. **Check if drivers are needed**
   - Connect M5Stack via USB
   - Open Device Manager
   - Look for "Silicon Labs CP210x USB to UART Bridge"

2. **If not found, download drivers**
   ```
   https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
   ```

3. **Install drivers**
   - Run the installer
   - Follow on-screen instructions
   - Restart computer if prompted

### Step 4: Install Git (Optional but Recommended)

#### Windows
```powershell
# Download from official website
https://git-scm.com/download/win

# Or use winget
winget install Git.Git
```

#### macOS
```bash
# Using Homebrew
brew install git

# Or use Xcode Command Line Tools
xcode-select --install
```

#### Linux
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install git

# Fedora
sudo dnf install git
```

---

## Project Setup

### Method 1: Using Git (Recommended)

1. **Open Terminal/Command Prompt**

2. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/road-condition-monitoring.git
   cd road-condition-monitoring
   ```

3. **Open in VS Code**
   ```bash
   code .
   ```

### Method 2: Download ZIP

1. **Download project**
   - Go to GitHub repository
   - Click "Code" → "Download ZIP"
   - Extract to desired location

2. **Open in VS Code**
   - File → Open Folder
   - Select extracted folder

### Step 2: Verify Project Structure

Your project should look like this:
```
road-condition-monitoring/
├── src/
│   └── main.cpp
├── include/
│   └── README
├── lib/
│   └── README
├── platformio.ini
├── README.md
└── docs/
    └── (documentation files)
```

### Step 3: Install Dependencies

PlatformIO will automatically install required libraries:
- M5Unified v0.1.16+
- TinyGPSPlus v1.0.3+
- ESP8266 Influxdb v3.13.1+

**To trigger installation:**
1. Open `platformio.ini`
2. Save the file
3. PlatformIO will start downloading dependencies
4. Watch the progress in the bottom status bar

---

## Configuration

### Step 1: Configure WiFi

1. **Open `src/main.cpp`**

2. **Locate WiFi settings** (around line 8-9):
   ```cpp
   #define WIFI_SSID "iPhone von Achraf"
   #define WIFI_PASSWORD "Hello2020"
   ```

3. **Replace with your WiFi credentials**:
   ```cpp
   #define WIFI_SSID "YourNetworkName"
   #define WIFI_PASSWORD "YourPassword"
   ```

**Important Notes:**
- Use 2.4GHz WiFi network (ESP32 doesn't support 5GHz)
- Avoid special characters in SSID if possible
- Ensure WiFi password is correct (case-sensitive)

### Step 2: Configure InfluxDB

The default configuration should work for Hochschule Osnabrück students:

```cpp
#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "bCfPoZ_Ys5WvjAztMdlBUioe7e_8N9vAbCuxPEWFGVhJEEr-IfI9b8QxNxMBXRCWY7hKFvWpI4ONSmFis5zwUQ=="
#define INFLUXDB_ORG "hsos"
#define INFLUXDB_BUCKET "iot-bucket"
```

**For custom InfluxDB setup:**
- Replace URL with your InfluxDB Cloud URL
- Generate new token in InfluxDB dashboard
- Set your organization name
- Set your bucket name

### Step 3: Set Unique Measurement Name

**CRITICAL**: Change measurement name to avoid data conflicts!

1. **Locate measurement name** (around line 17):
   ```cpp
   #define MEASUREMENT_NAME "group_Ahmed_Achraf_Bouker"
   ```

2. **Change to your unique identifier**:
   ```cpp
   #define MEASUREMENT_NAME "group_YourName_YourPartner"
   ```

**Naming suggestions:**
- Use your names: `group_John_Jane`
- Use student IDs: `group_12345_67890`
- Use team name: `team_alpha`
- Use random identifier: `device_abc123`

### Step 4: Verify GPS Settings

Default settings should work, but verify:

```cpp
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_BAUD 9600
```

**Only change if:**
- Using different hardware configuration
- GPS module has different baud rate
- Custom pin assignment needed

---

## First Run

### Step 1: Connect Hardware

1. **Connect M5Stack to computer**
   - Use USB-C cable
   - Ensure good connection
   - LED should light up

2. **Check device recognition**
   - **Windows**: Device Manager → Ports (COM & LPT)
   - **macOS**: Terminal → `ls /dev/cu.*`
   - **Linux**: Terminal → `ls /dev/ttyUSB*`

### Step 2: Build Project

1. **Open PlatformIO panel**
   - Click PlatformIO icon in sidebar

2. **Expand "m5stack-core-esp32" environment**

3. **Click "Build"**
   - Or press `Ctrl+Alt+B` (Windows/Linux)
   - Or press `Cmd+Alt+B` (macOS)

4. **Wait for compilation**
   - First build takes longer (downloading tools)
   - Watch for errors in terminal
   - Should end with "SUCCESS"

**If build fails:**
- Check internet connection
- Verify `platformio.ini` is correct
- Try: PlatformIO → Clean
- Delete `.pio` folder and rebuild

### Step 3: Upload to Device

1. **Select upload task**
   - PlatformIO panel → Upload
   - Or press `Ctrl+Alt+U` (Windows/Linux)
   - Or press `Cmd+Alt+U` (macOS)

2. **Monitor upload progress**
   - Should show "Connecting..."
   - Then uploading progress bars
   - Finally "SUCCESS"

**If upload fails:**
- Try holding Button A during upload
- Check USB cable (use data cable, not charge-only)
- Verify correct COM port selected
- Try different USB port
- Restart M5Stack (red button on side)

### Step 4: Open Serial Monitor

1. **Start serial monitor**
   - PlatformIO panel → Monitor
   - Or press `Ctrl+Alt+S` (Windows/Linux)
   - Or press `Cmd+Alt+S` (macOS)

2. **You should see:**
   ```
   === GPS + IMU + InfluxDB ===
   GPS initialized
   Connecting to WiFi: YourNetworkName
   ...........
   WiFi Connected!
   IP: 192.168.1.100
   InfluxDB: Connected
   
   System Ready!
   Press Button A to set start position
   ```

---

## Verification

### Test 1: WiFi Connection

**Expected behavior:**
- "WiFi: Connecting..." appears on display
- Within 10 seconds: "WiFi: Connected!" (green)
- Serial monitor shows IP address

**If fails:**
- Check WiFi credentials
- Move closer to router
- Ensure 2.4GHz network
- Check router allows new devices

### Test 2: InfluxDB Connection

**Expected behavior:**
- "InfluxDB: Connected" (green) on display
- Serial monitor confirms connection
- No error messages

**If fails:**
- Check internet connectivity
- Verify InfluxDB token
- Check URL is correct
- Try browser: can you access InfluxDB URL?

### Test 3: GPS Lock

**Expected behavior:**
- Within 1-2 minutes outdoors
- Satellite count increases (visible on display)
- HDOP value decreases below 5
- Valid coordinates appear

**If fails:**
- Move outdoors with clear sky view
- Check antenna connection (black cable)
- Wait longer (cold start can take 2-3 minutes)
- Try repositioning antenna

### Test 4: IMU Reading

**Expected behavior:**
- Acceleration values change when moving device
- At rest: approximately (0, 0, 1000) mg
- When moving: values fluctuate
- Status shows "Smooth ride" when still

**If fails:**
- Restart device
- Check I2C connections
- Re-upload firmware

### Test 5: Data Logging

**Expected behavior:**
- Every 5 seconds: "✓ Data sent to InfluxDB" in serial
- No write errors

**If fails:**
- Check WiFi still connected
- Verify InfluxDB credentials
- Check bucket exists
- Try refreshing InfluxDB dashboard

### Test 6: Display Update

**Expected behavior:**
- Screen refreshes every 1 second
- All data fields populate
- No frozen or garbled text
- Button instruction visible

**If fails:**
- Reset device
- Check M5Unified library version
- Re-upload firmware

---

## Outdoor Testing

### Pre-flight Checklist

Before going outdoors:
- [ ] WiFi configured and tested
- [ ] InfluxDB connection verified
- [ ] Battery charged (or power bank connected)
- [ ] GPS antenna properly connected
- [ ] Serial monitor shows "System Ready!"
- [ ] Screenshot capability ready (for assignment)

### Field Test Procedure

1. **Initial Setup (Indoors)**
   - Power on device
   - Verify WiFi connection
   - Check InfluxDB connectivity

2. **Move Outdoors**
   - Find location with clear sky view
   - Avoid tall buildings, trees
   - Wait for GPS lock (1-2 minutes)

3. **Set Start Position**
   - Press Button A when GPS locked
   - Verify "START POSITION SET!" in serial
   - Display shows time and distance from start

4. **Begin Monitoring**
   - Walk or cycle on road
   - Observe display for:
     - Changing coordinates
     - Increasing distance
     - Road condition status
   - Try going over bumps/potholes

5. **Verify Data Collection**
   - Check serial monitor for "Data sent" messages
   - No error messages appearing
   - GPS coordinates updating

6. **Review in InfluxDB**
   - Access InfluxDB dashboard
   - Select your measurement
   - Verify data appearing in real-time
   - Take screenshots for assignment

---

## Troubleshooting Common Issues

### Issue: "WiFi: FAILED" on Display

**Solutions:**
1. Check SSID and password spelling
2. Ensure 2.4GHz network
3. Move closer to router
4. Restart router
5. Try mobile hotspot as test

### Issue: "InfluxDB: Error" on Display

**Solutions:**
1. Check internet connection
2. Verify token hasn't expired
3. Test URL in browser
4. Check organization and bucket names
5. Ensure measurement name is unique

### Issue: GPS Shows 0 Satellites

**Solutions:**
1. Move outdoors
2. Check antenna connection (black cable!)
3. Wait 2-3 minutes for cold start
4. Ensure antenna has sky view
5. Try repositioning antenna
6. Check GPS module is firmly attached

### Issue: Upload Fails

**Solutions:**
1. Hold Button A during upload
2. Try different USB cable
3. Check correct COM port selected
4. Install/update USB drivers
5. Try different USB port on computer
6. Restart M5Stack and computer

### Issue: Display Shows Garbled Text

**Solutions:**
1. Reset M5Stack (red button)
2. Re-upload firmware
3. Check M5Unified library version
4. Update to latest version
5. Clean and rebuild project

### Issue: High HDOP (> 5.0)

**Solutions:**
1. Wait for more satellites
2. Move to more open area
3. Check for obstructions
4. Normal in urban environments
5. May improve over time

---

## Next Steps

Once everything is verified and working:

1. **Complete Assignment Questions**
   - Answer all 7 questions in ASSIGNMENT.md
   - Include screenshots from InfluxDB
   - Document any difficulties

2. **Conduct Field Tests**
   - Test on bicycle or vehicle
   - Try different road conditions
   - Collect meaningful data

3. **Analyze Data**
   - Create graphs in InfluxDB
   - Identify bump patterns
   - Compare different routes

4. **Optional Enhancements**
   - Adjust bump detection thresholds
   - Add additional sensors
   - Implement SD card logging
   - Create custom visualizations

---

## Getting Help

### Resources
- **Project README**: Complete overview
- **TECHNICAL_DOCS**: Detailed technical information
- **ASSIGNMENT**: Original assignment requirements
- **GitHub Issues**: Report bugs and ask questions

### Contact
- **Course Instructors**: During lab hours (Mondays 18:00-19:30)
- **Email Support**: t.seidel@hs-osnabrueck.de, simon.balzer@hs-osnabrueck.de
- **Project Author**: Abdelrahman Ahmed

### Community
- **M5Stack Community**: https://community.m5stack.com/
- **PlatformIO Forum**: https://community.platformio.org/
- **Arduino Forum**: https://forum.arduino.cc/

---

## Checklist Summary

Before considering setup complete:

- [ ] Hardware assembled correctly
- [ ] VS Code and PlatformIO installed
- [ ] Project downloaded/cloned
- [ ] WiFi credentials configured
- [ ] Unique measurement name set
- [ ] Project builds successfully
- [ ] Firmware uploads to device
- [ ] WiFi connects successfully
- [ ] InfluxDB connection verified
- [ ] GPS gets satellite lock
- [ ] IMU reads acceleration data
- [ ] Display updates correctly
- [ ] Data appears in InfluxDB
- [ ] Serial monitor shows no errors
- [ ] Outdoor test successful

---

**Setup Guide Version**: 1.0  
**Last Updated**: November 2024  
**Author**: Abdelrahman Ahmed
