#ifndef RIFD_HANDLER_H
#define RIFD_HANDLER_H

#include "rc522.h"
#include "driver/rc522_spi.h"
#include "rc522_picc.h"
#include "task_common.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "UARTHandler.h"
#include "Global.h"

#define RC522_SPI_BUS_GPIO_MISO    (19)
#define RC522_SPI_BUS_GPIO_MOSI    (21)
#define RC522_SPI_BUS_GPIO_SCLK    (22)
#define RC522_SPI_SCANNER_GPIO_SDA (23)
#define RC522_SCANNER_GPIO_RST     (-1) // soft-reset

extern rc522_driver_handle_t driver;
extern rc522_handle_t scanner;
extern rc522_spi_config_t driver_config;


esp_err_t init_nvs(void);
void continuous_read_task(void *arg);
esp_err_t hex_string_to_bytes(const char* hex_string, uint8_t* bytes, size_t max_len);
esp_err_t write_to_rfid_card(const char* data);
void save_rfid_data_to_nvs(void);
void read_rfid_data_from_nvs(void);
void on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data);

#endif