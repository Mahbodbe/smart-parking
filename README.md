# 🚗 Smart Parking DAQ System & IoT Control

> **Course:** Instrumentation and Measurement  
> **Instructor:** Dr. Ahmad Afshar  
> **Institution:** Amirkabir University of Technology (Tehran Polytechnic) — Department of Electrical Engineering  

---

<!-- Badges Section for Tech Stack and Languages -->
<p align="center">
  <img src="https://img.shields.io/badge/Microcontroller-ESP32-E67E22?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32">
  <img src="https://img.shields.io/badge/Backend-Python-3776AB?style=for-the-badge&logo=python&logoColor=white" alt="Python">
  <img src="https://img.shields.io/badge/Firmware-C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++">
  <img src="https://img.shields.io/badge/Simulation-Proteus-004088?style=for-the-badge" alt="Proteus">
</p>

<p align="center">
  <a href="#-english-version">
    <img src="https://img.shields.io/badge/Language-English-blue.svg?style=flat-square" alt="English">
  </a>
  <a href="README_FA.md">
    <img src="https://img.shields.io/badge/Language-%D9%81%D8%A7%D8%B1%D8%B3%DB%8C-green.svg?style=flat-square" alt="فارسی">
  </a>
</p>

---

## 👥 Project Team

| Member Name | GitHub Profile |
| :--- | :--- |
| **Mahbod BemaniCham** | [@Mahbodbe](https://github.com/Mahbodbe) |
| **Erfan Ashkesh** | [@THEEA8](https://github.com/THEEA8) |
| **Mani Mohammadi** | [@loManiol](https://github.com/loManiol) |

---

## 🌐 English Version

### 📌 Project Overview
The **Smart Parking DAQ System** is an end-to-end IoT and Instrumentation solution engineered for automated multi-story parking structures. It seamlessly integrates multi-sensor physical data acquisition (RFID, Ultrasonic, Hall Effect, Temperature/Humidity, IR Break-beam), continuous-time closed-loop barrier gate control via a PID-tuned servo motor, automated two-factor vehicle authentication (ANPR + RFID), and real-time telemetry streaming over a central Python web dashboard and a Telegram notification bot.

---

### 🚀 Key Features

* **Two-Factor Vehicle Authentication:** High-security dual-layer verification combining an **RC522 13.56MHz RFID** card UID check with an ANPR Camera system (license plate validation & vehicle color matching).
* **Automated Nearest-Slot Allocation:** Real-time database query executing an optimized algorithm to prioritize the closest vacant parking slot sorted dynamically by floor level:

$$\text{Target Slot} = \arg\min_{\{\text{floor, slot}\}} \{ \text{floor} \mid \text{Occupied} = 0 \}$$

* **Multi-Layer Hardware Redundancy:** 
  * *Dual Occupancy Sensing:* Cross-verification via HC-SR04 Ultrasonic Distance Sensors and A3144 Hall Effect Sensors.
  * *Dual Power Supply:* Seamless hardware switchover between the main AC Grid and an Online UPS Battery backup.
  * *Dual Data Persistence:* Centralized SQLite/PostgreSQL cloud sync backed up by local ESP32 EEPROM/SD Card buffering to prevent data loss during network outages.
* **Closed-Loop Servo Gate Control:** Precise PID-driven barrier arm positioning ($\%OS < 5\%$, $T_s < 0.32\text{s}$) paired with an active anti-pinch IR break-beam threshold safety trigger.
* **Interactive Web GUI & Telemetry Dashboard:** Live KPI tracking cards, a 60-slot visual grid layout, real-time Chart.js environmental telemetry updates, and live access logging.
* **Proteus Hardware Co-Simulation:** Full-scale structural hardware simulation inside Proteus interconnected with the physical ESP32 MCU via a COMPIM virtual serial bridge.

---

### 📊 Web Dashboard & System Results

| Real-Time KPIs, Occupancy Map & Environmental Telemetry | Live Vehicle Access Logs & Slot Grid |
| :---: | :---: |
| ![KPIs, Occupancy Map & Telemetry](report/dashboard-kpi-occupancy-telemetry.jpg) | ![Live Access Logs & Slot Grid](report/dashboard-live-access-logs.jpg) |
| *Live counters (Inside, Vacant, Entry, Exit), 60-Slot Occupancy Grid (Green=Vacant, Red=Occupied), and Real-Time Temp/Humidity Charts.* | *Real-time access log table featuring Timestamp, UID, License Plate, Action Badge (Entry/Exit), and Allocated Slot ID.* |

---

### ⚠️ ESP32 Firmware Setup & Configuration

> [!CRITICAL]
> **Wi-Fi & Server Credentials Configuration**  
> Before flashing the microcontroller firmware located inside the `arduinocode/` or `rc522sevorgbwifi/` directories, you **MUST** update your local network configurations and Central Python Server IP address inside the primary source code file (`.ino` / `.cpp`):
>
> ```cpp
> // Replace these placeholders with your actual network parameters:
> const char* ssid      = "YOUR_WIFI_SSID";
> const char* password  = "YOUR_WIFI_PASSWORD";
> const char* serverUrl = "http://YOUR_SERVER_IP:8000/api/gate/";
> ```

---

### 📂 Repository Structure

```text
smart-parking/
├── arduinocode/          # ESP32 Main Firmware (Sensors, Servo, LCD, Wi-Fi Drivers)
├── rc522sevorgbwifi/     # Isolated Hardware Test Modules (RFID & RGB LED)
├── proteus-simulation/   # Proteus Hardware Co-Simulation Schematic Files (.pdsprj)
└── report/               # Technical Engineering Documentation & Assets
    ├── dashboard-kpi-occupancy-telemetry.jpg
    ├── dashboard-live-access-logs.jpg
    ├── instr-parking-final-report.pdf    # Full Comprehensive Technical Report (Persian)
    └── instr-parking-final-report-en.pdf # Full Comprehensive Technical Report (English)
```

---

### 📖 Engineering Documentation
For detailed mathematical modeling, PID loop transfer functions, signal conditioning circuit schematics (including 10kΩ pull-up calculations), and system security topologies, please consult the complete PDF reports located inside the `report/` directory.
