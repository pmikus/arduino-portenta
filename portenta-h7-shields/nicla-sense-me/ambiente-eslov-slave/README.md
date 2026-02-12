# Ambiente Eslov Slave - Arduino Nicla Sense ME

A lightweight sensor firmware for the Nicla Sense ME board that continuously reads environmental sensor data and streams it via ESLOV (I2C) to a host board.

## Overview

This sketch runs on the **Nicla Sense ME** board as a slave device. It initializes all onboard environmental sensors and continuously updates measurements, making them available to a host board (like Arduino Portenta H7) via the ESLOV I2C interface.

This is typically paired with the **Master** sketch (`ambiente-eslov-master.ino`) running on a Portenta H7 or similar host device that collects and processes the sensor data.

## Hardware Requirements

- **Nicla Sense ME** - Environmental sensor board with multi-sensor IC
- **ESLOV Connector** - Connected to host board (e.g., Arduino Portenta H7)
- **Power Supply** - Provided through ESLOV connector from host board

## Features

### Environmental Sensors

The Nicla Sense ME includes multiple integrated sensors:

- **Temperature Sensor** - Environmental temperature measurement
- **Humidity Sensor** - Relative humidity measurement
- **Pressure Sensor** - Barometric pressure sensing
- **Gas Sensor** - VOC and CO₂ detection
- **BSEC Sensor** - Indoor Air Quality (IAQ) calculation
- **IMU (optional)** - Inertial Measurement Unit (accelerometer, gyroscope)

### Connectivity

- **ESLOV I2C Interface** - Direct communication with host board via standard I2C protocol
- **BLE Support** - Optional Bluetooth Low Energy connectivity (can be configured)
- **Low Power** - Minimal firmware footprint for efficient operation

## Installation

### 1. Arduino Setup

1. Install Arduino IDE or Arduino Cloud Editor
2. Add Nicla Sense ME board to your IDE:
   - Board Manager → Search "Nicla Sense ME" → Install
3. Select board: **Arduino Nicla Sense ME**
4. Select port: USB port where Nicla Sense ME is connected (for programming only)

### 2. Required Libraries

Install this library via Arduino Library Manager:

- `Arduino_BHY2` - Nicla Sense ME sensor support and ESLOV communication

Additional libraries may be automatically installed as dependencies.

### 3. Upload

1. Upload the `ambiente-eslov-slave.ino` sketch to the Nicla Sense ME board via USB
2. Once uploaded, the board does not need USB connection during normal operation
3. Connect the Nicla Sense ME via ESLOV connector to your host board

## Usage

### Normal Operation

Once the sketch is running on the Nicla Sense ME:

1. **ESLOV Connection** - Sensor data is continuously streamed via I2C to the connected host board
2. **No Additional Configuration** - The slave board continuously updates all sensor readings
3. **Host Communication** - The host board (Master) reads sensor values at its own interval

### Serial Debug Mode

To enable debug output via the Nicla Sense ME's USB connection:

1. Modify the sketch:
```cpp
#define DEBUG true
```

2. Upload and open Serial Monitor (9600 baud)

3. Debug output will show:
   - Sensor initialization status
   - Sensor readings and updates
   - I2C/ESLOV communication details
   - Error messages if any sensors fail

When debug mode is disabled (`#define DEBUG false`), the sketch runs silently and uses minimal resources.

## Configuration

### Debug Setting

```cpp
// Set DEBUG to true in order to enable debug print
#define DEBUG false
```

- `false` (default) - Runs silently, optimized for power consumption
- `true` - Prints sensor data and debug information to Serial Monitor at 9600 baud

### ESLOV Interface Configuration

```cpp
BHY2.begin(NICLA_BLE_AND_I2C, NICLA_VIA_ESLOV);
```

- `NICLA_BLE_AND_I2C` - Enables both BLE and I2C (ESLOV) communication
- `NICLA_VIA_ESLOV` - Routes data through ESLOV connector to host board

### Sensor Update Rate

```cpp
BHY2.update(5000);
```

- Parameter `5000` represents the update interval in milliseconds (5 seconds)
- Lower values increase responsiveness but use more power
- Adjust based on your application requirements

## Pinout

The Nicla Sense ME connects to the host board via the **ESLOV connector**:

| ESLOV Pin | Function | Description |
|-----------|----------|-------------|
| SDA | I2C Data | I2C data line |
| SCL | I2C Clock | I2C clock line |
| GND | Ground | System ground |
| 5V/3V3 | Power | Power supply from host |

**Note:** Exact pin names depend on your host board. Refer to your host board's documentation.

## System Architecture

```
┌─────────────────────┐
│  Host Board         │
│ (Portenta H7)       │
│                     │
│  ├─ Master Sketch   │
│  ├─ WiFi/Cloud      │
│  └─ Web Server      │
└──────────┬──────────┘
           │ ESLOV (I2C)
           │ SDA/SCL
           │
┌──────────┴──────────┐
│  Slave Board        │
│ (Nicla Sense ME)    │
│                     │
│  ├─ Slave Sketch    │
│  ├─ Temperature     │
│  ├─ Humidity        │
│  ├─ Pressure        │
│  ├─ Gas             │
│  └─ IAQ             │
└─────────────────────┘
```

## Workflow

1. **Initialization:** On startup, `BHY2.begin()` initializes all onboard sensors
2. **Continuous Update:** The `loop()` function calls `BHY2.update()` every iteration
3. **I2C Ready:** Sensor data is held in BHY2 registers, accessible via I2C by the host board
4. **Host Reads:** The master board (running `ambiente-eslov-master`) periodically reads sensor values via I2C
5. **Cloud Sync:** Master processes the data and sends it to Arduino IoT Cloud

## Power Consumption

- **Active Mode:** ~50-100 mA (depending on enabled sensors)
- **Debug Mode:** Higher due to Serial output and additional processing
- **Low Power:** Can be optimized further with adjusting update intervals

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Arduino IDE doesn't detect Nicla Sense ME | Install board drivers, check USB cable, try different USB port |
| Sensors not initializing | Verify `Arduino_BHY2` library is installed, check USB/ESLOV connection |
| No communication with host board | Ensure ESLOV connector is properly seated, check I2C pullup resistors on host |
| Debug output not appearing | Verify `DEBUG` is set to `true`, open Serial Monitor at 9600 baud |
| Host cannot read sensor values | Check I2C addresses, verify both boards have proper power supply |

## Master-Slave Communication

This slave sketch is designed to work with the **Master** sketch. The master board:

1. Reads sensor values from this slave via I2C
2. Applies calibration formulas to raw values
3. Sends processed data to Arduino IoT Cloud
4. Hosts a local web interface showing sensor readings

For complete environmental monitoring system setup, see the master sketch documentation.

## Technical Specifications

- **Microcontroller:** Arm Cortex-M4
- **I2C Address:** Configurable (depends on BHY2 library defaults)
- **Communication Speed:** ESLOV I2C standard speeds
- **Update Interval:** 5 seconds (configurable)
- **Serial Baud Rate (Debug):** 9600 baud

## License

See LICENSE file in this directory.

## References

- [Arduino Nicla Sense ME Documentation](https://docs.arduino.cc/hardware/nicla-sense-me)
- [Bosch BHY2 Sensor Hub](https://www.bosch-sensortec.com/software-tools/software/bhy2-host-driver/)
- [ESLOV Connector Specification](https://docs.arduino.cc/hardware/nicla-sense-me#eslov)
- [Arduino_BHY2 Library](https://github.com/arduino-libraries/Arduino_BHY2)
