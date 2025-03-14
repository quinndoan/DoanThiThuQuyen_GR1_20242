#include <esp_log.h>
#include "rc522.h"
#include "driver/rc522_spi.h"
#include "rc522_picc.h"
#include "task_common.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "Global.h"
#include "UARTHandler.h"
#include "RIFD_Handler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "rc522_reading_card";

#define RC522_SPI_BUS_GPIO_MISO    (19)
#define RC522_SPI_BUS_GPIO_MOSI    (21)
#define RC522_SPI_BUS_GPIO_SCLK    (22)
#define RC522_SPI_SCANNER_GPIO_SDA (23)
#define RC522_SCANNER_GPIO_RST     (-1) // soft-reset

// Định nghĩa biến global
char g_uid[20];
char g_atqa[10];
char g_sak[10];

extern rc522_spi_config_t driver_config;
extern rc522_driver_handle_t driver;
extern rc522_handle_t scanner;


void app_main()
{
    // Khởi tạo NVS
    ESP_ERROR_CHECK(init_nvs());
    void initialize_uart();
    // Đọc dữ liệu đã lưu (nếu có)
    read_rfid_data_from_nvs();
    
    // Khởi tạo RC522
    rc522_spi_create(&driver_config, &driver);
    rc522_driver_install(driver);

    rc522_config_t scanner_config = {
        .driver = driver,
    };

    rc522_create(&scanner_config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_picc_state_changed, NULL);
    rc522_start(scanner);
    
    // Khởi tạo UART
   // void initialize_uart();
    
    // Tạo task đọc liên tục thẻ RFID
    xTaskCreate(continuous_read_task, "rfid_read_task", 4096, NULL, 5, NULL);
    
    // Tạo task UART
    xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, 5, NULL);
}
