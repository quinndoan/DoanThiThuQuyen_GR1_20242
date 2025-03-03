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

char g_uid[20];
char g_atqa[10];
char g_sak[10];


extern rc522_driver_handle_t driver;
extern rc522_handle_t scanner;
extern rc522_spi_config_t driver_config;

void app_main()
{
    // Initialize UART
    initialize_uart();
    ESP_ERROR_CHECK(init_nvs());
    
    read_rfid_data_from_nvs();
    
    // Create UART receive task to handle incoming commands
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    // Start RC522
    rc522_spi_create(&driver_config, &driver);
    rc522_driver_install(driver);

    rc522_config_t scanner_config = {
        .driver = driver,
    };

    rc522_create(&scanner_config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_picc_state_changed, NULL); // handle variable, trigger, handle function, null
    rc522_start(scanner);
    
    // Tạo task đọc liên tục thẻ RFID
    xTaskCreate(continuous_read_task, "rfid_read_task", 4096, NULL, 5, NULL);
}
