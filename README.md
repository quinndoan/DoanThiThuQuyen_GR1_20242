# ESP32 Dual-Frequency RFID Reader with OLED and MQTT Support

This project is a firmware application for the ESP32 family of microcontrollers. It enables dual RFID reading, supports real-time display via an OLED screen, and communicates with external systems using MQTT.

## Supported Targets

| ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
|-------|----------|----------|----------|----------|----------|----------|----------|----------|

## Features

- Dual RFID Support
  - 125 kHz RFID (e.g., EM4100): Read-only, communicates via UART
  - 13.56 MHz RFID (e.g., RC522/MIFARE): Read/Write, communicates via SPI, receives commands via UART from a PC and sends responses accordingly

- OLED Display
  - Real-time display of scanned tag data and system messages

- MQTT Integration
  - The ESP32 can both publish and subscribe to topics for remote monitoring and control

## Requirements

- ESP32 Development Board  
- 125 kHz RFID Reader (UART)  
- 13.56 MHz RFID Reader (SPI, e.g., RC522)  
- OLED Display (SSD1306)  
- USB-to-UART Converter  
- MQTT Broker (e.g., Mosquitto, HiveMQ)  
- ESP-IDF and VSCode  

## Project Structure
```

├── components/
│ └── ssd1306/                  # OLED driver
├── esp-idf-rc522-main/         # RC522 13.56 MHz RFID driver
├── main/                       # Main application source
├── CMakeLists.txt              # Top-level CMake config
├── sdkconfig                   # ESP-IDF project config
└── README.md
```
## Build and Configuration Notes

This project uses CMake via the ESP-IDF build system. Keep the following in mind:

- All `.c` source files in `main/` must be declared in `main/CMakeLists.txt`
- When adding third-party drivers or large modules, place them in a separate folder and register it via `EXTRA_COMPONENT_DIRS` in the top-level `CMakeLists.txt`
- Each component (e.g., `ssd1306`, `esp-idf-rc522-main`) must have its own `CMakeLists.txt` that defines its sources and include paths. Refer to existing examples in:
    - `components/ssd1306/CMakeLists.txt`
    - `esp-idf-rc522-main/CMakeLists.txt`