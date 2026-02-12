# Arduino Portenta

A collection of experimental and production projects spanning IoT, embedded systems, and cloud infrastructure automation.

## Projects

- **Nicla Sense ME BLE Dashboard** (`nicla-sense-me-base/`)
  - Bluetooth Low Energy (BLE) dashboard for Arduino Nicla Sense ME sensor board
  - Exposes environmental sensors (temperature, humidity, CO2, gas readings) over BLE
  - Compatible with web-based dashboard for real-time monitoring
  - Features: accelerometer, gyroscope, quaternion orientation tracking, RGB LED control

- **Portenta H7 Weather Station** (`portenta-h7-base/`)
  - WiFi-enabled weather application for Arduino Portenta H7
  - SSL/TLS secure connections using ArduinoBearSSL
  - JSON-based API integration for weather data
  - Includes Arduino secrets and trust anchor configuration

- **Portenta H7 Shields** (`portenta-h7-shields/`)
  - Environment sensor integrations with Ambiente ESLOV shield
  - Master/Slave configuration options for distributed sensing
  - Multi-sensor data collection and reporting

**Key Features:**
- Arduino IDE compatible sketches
- BLE and WiFi connectivity
- Sensor integration (environmental, motion, orientation)
- Secure communication with SSL/TLS support

**Documentation:**
![Portenta H7 Pinout](https://content.arduino.cc/assets/Pinout-PortentaH7_latest.png)
![Nicla Sense ME Pinout](https://docs.arduino.cc/static/e247ef5a7bb395c6644146a36568b9dc/2f891/AKX00050-pinout.png)

## Getting Started

### Arduino Projects
1. Clone the repository
2. Open the `.ino` sketch files in Arduino IDE
3. Configure secrets in `arduino_secrets.h` from the `.example` file
4. Select appropriate board and port
5. Upload to your Portenta H7 or Nicla Sense ME device

## Requirements

### Arduino Projects
- Arduino IDE 2.x
- Arduino Portenta H7 or Nicla Sense ME board
- Appropriate Arduino libraries (ArduinoJSON, ArduinoBLE, etc.)