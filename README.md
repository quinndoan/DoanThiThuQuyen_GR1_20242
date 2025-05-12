| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

## Overview and Features

This project is a firmware application for the ESP32 microcontroller that supports reading RFID tags of two different frequency types:

    - 125 kHz RFID (e.g., EM4100) — Read-only, communicates via UART
    - 13.56 MHz RFID (e.g., MIFARE) — Read and write, communicates via SPI

In addition to RFID capabilities, the firmware supports MQTT communication, allowing the ESP32 to both publish and subscribe to messages on a specified topic.

## Requirements

    - ESP32 Development Board
    - 125 kHz RFID reader (UART)
    - 13.56 MHz RFID reader (SPI, e.g., RC522)
    - MQTT Broker (e.g., Mosquitto)
    - Vscode and ESP-IDF toolchain

## Folder Structure

```
main/
├── components/
├── esp-idf-rc522-main/
│ ├── CMakeLists.txt
│ ├── Global.h
│ ├── main.c
│ ├── MQTTHandler.c
│ ├── MQTTHandler.h
│ ├── rc522_Handler.c
│ ├── rc522_Handler.h
│ ├── task_common.h
│ ├── UARTHandler.c
│ ├── UARTHandler.h
│ ├── wifiHandler.c
│ └── wifiHandler.h
├── CMakeLists.txt
├── README.md
└── sdkconfig
```

## Configuration