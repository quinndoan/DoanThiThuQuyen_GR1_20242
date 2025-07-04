# Objective

This project is a firmware application for the ESP32 family of microcontrollers. It enables dual RFID reading, supports real-time display via an OLED screen, and communicates with external systems using MQTT.

## Features

- Dual RFID Support
  - 125 kHz RFID (RDM6300): Read-only from Card, communicates via UART
  - 13.56 MHz RFID (RC522/MIFARE): Read/Write to Card, communicates via SPI, receives commands via UART from a PC and sends responses accordingly

- OLED Display
  - Real-time display of scanned tag data from 125kHz RFID

- MQTT Integration
  - The ESP32 can both publish and subscribe to MQTT topics
  - Enables easy integration with external systems and supports future project expansion and remote monitoring features

## Hardware Requirements and Evironment

- ESP32 Development Board  
- 125 kHz RFID Reader (UART, e.g., RDM6300)  
- 13.56 MHz RFID Reader (SPI, e.g., RC522)  
- OLED Display (I2C, e.g., SSD1306)  
- USB-to-UART Converter  
- MQTT Broker (e.g., Mosquitto, HiveMQ)  
- ESP-IDF and VSCode  

## Schematic
|ESP32|Peripheral|
|--|--|
|D4|UART_Rx|
|D5|UART_Tx|
|D16|RDM6300_Tx|
|D17|RDM6300_Rx|
|D18|RC522_MISO|
|D19|RC522_MOSI|
|D26|RC522_SCK|
|D27|RC522_SDA|
|D22|I2C_SCL|
|D21|I2C_SDA|

## Function Specification

This section explains the key functions implemented in the project and their roles in the overall system.

```C
/**
 * Handle the event when the RFID card (PICC) state changes,
 * such as when a card is placed on or removed from the reader.
 * Stores UID, ATQA, and SAK into global variables and NVS when a card is detected.
 *
 * @param arg       Unused argument.
 * @param base      Event base indicating the type of event.
 * @param event_id  Event ID.
 * @param data      Pointer to event data containing RFID card information.
 */
void on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data);
```
```C
/**
 * Task that receives data from UART.
 * Reads user commands sent via UART and forwards them to the command handler.
 *
 * @param arg  Unused argument.
 */
void rx_task(void *arg);
```
```C
/**
 * Task for reading data from a 125KHz RFID reader (RDM6300).
 * Displays the data on an OLED screen and publishes it to MQTT if connected.
 *
 * @param arg  Unused argument.
 */
void RFID_125_Task(void *arg);
```
```C
/**
 * Handles MQTT-related events such as connection, data reception, and disconnection.
 * Used to update connection status and handle incoming messages from the broker.
 *
 * @param handler_args  Handler argument.
 * @param base          Event base indicating the type of event.
 * @param event_id      Event ID.
 * @param event_data    Event data containing MQTT event information.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
```

## Build and Configuration Notes

This project uses CMake via the ESP-IDF build system. Keep the following in mind:

- All `.c` source files in `main/` must be declared in `main/CMakeLists.txt`
- When adding third-party drivers or large modules, place them in a separate folder and register it via `EXTRA_COMPONENT_DIRS` in the top-level `CMakeLists.txt`
- Each component (e.g., `ssd1306`, `esp-idf-rc522-main`) must have its own `CMakeLists.txt` that defines its sources and include paths. Refer to existing examples in:
    - `components/ssd1306/CMakeLists.txt`
    - `esp-idf-rc522-main/CMakeLists.txt`

## Result
This section demonstrates the results of running the project with its core functionalities.

- Reading Card Info Using RC522
![Reading Card Info Using RC522](https://github.com/quinndoan/DoanThiThuQuyen_GR1_20242/blob/main/image/rc522_readingfromcard.png)

- Read and Write to Device Using UART
![Read and Write to Device Using UART](https://github.com/quinndoan/DoanThiThuQuyen_GR1_20242/blob/main/image/rc522_uart.png)

- Reading Card Info Using RDM6300 and Connect to MQTT
![Reading Card Info Using RDM6300 and Connect to MQTT](https://github.com/quinndoan/DoanThiThuQuyen_GR1_20242/blob/main/image/rdm6300_mqtt.png)

- Display on OLED Screen
![OLed](https://github.com/quinndoan/DoanThiThuQuyen_GR1_20242/blob/main/image/oled.jpg)

