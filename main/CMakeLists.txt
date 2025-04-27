idf_component_register(SRCS "UARTHandler.c" "main.c" "rc522_Handler.c"
                    INCLUDE_DIRS "."
                    REQUIRES "esp-idf-rc522-main"
                             "nvs_flash"
                             "driver"
                             "esp_event"
                             "ssd1306"
                             )


