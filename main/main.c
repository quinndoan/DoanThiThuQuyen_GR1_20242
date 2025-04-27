#include <esp_log.h>
#include "rc522.h"
#include "driver/uart.h"
#include "driver/rc522_spi.h"
#include "rc522_picc.h"
#include "task_common.h"
#include "nvs_flash.h"
#include "string.h"
#include "nvs.h"
#include "rc522_handler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ssd1306.h"
#include "font8x8_basic.h"

static const char *TAG = "main";

extern rc522_spi_config_t driver_config;
extern rc522_driver_handle_t driver;
extern rc522_handle_t scanner;
char g_atqa[10];
char g_sak[10];
char g_uid[20];

#define tag "SSD1306"

void app_main()
{
    
    
   initialize_uart_command();
   init_uart_for_RFID_125KHz();
   SSD1306_t dev;
	int center, top, bottom;
	char lineChar[20];

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(tag, "INTERFACE is i2c");
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

    ESP_ERROR_CHECK(init_nvs());
    // Đọc dữ liệu đã lưu (nếu có)
    read_rfid_data_from_nvs();
    // Start RC522
    rc522_spi_create(&driver_config, &driver);
    rc522_driver_install(driver);

    rc522_config_t scanner_config = {
        .driver = driver,
    };

    rc522_create(&scanner_config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_picc_state_changed, NULL);
    rc522_start(scanner);

    // Create UART receive task to handle incoming commands
    xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, configMAX_PRIORITIES-1, NULL);
    vTaskDelay(50);
    xTaskCreate(RFID_125_Task, "RFID_125_Task", 4096, NULL, configMAX_PRIORITIES-1, NULL);
   
}


