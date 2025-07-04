#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/queue.h"
#include "UARTHandler.h"
#include "task_common.h"
#include <driver/uart.h>
#include <string.h>
#include "rc522_Handler.h"
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "Global.h"
#include "MQTTHandler.h"

extern bool is_mqtt_connected;
static const char* TAG = "uartTask";
extern char g_uid[20];
extern char g_atqa[10];
extern char g_sak[10];
extern esp_mqtt_client_handle_t mqtt_client;
//extern rc522_picc_t *active_picc;
extern rc522_handle_t scanner;
extern rc522_picc_t *picc;
extern SSD1306_t dev;

// extern QueueHandle_t uart_queue_rfid_125KHz;

void init_uart_for_RFID_125KHz() {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_PORT_RDM6300, &uart_config);
    uart_set_pin(UART_PORT_RDM6300, TX_PIN_RDM6300, RX_PIN_RDM6300, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT_RDM6300, 1024, 0, 0, NULL, 0);
}


void initialize_uart_command() {
    const uart_config_t uart_config = {
         .baud_rate = 115200,
         .data_bits = UART_DATA_8_BITS,
         .parity = UART_PARITY_DISABLE,
         .stop_bits = UART_STOP_BITS_1,
         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
         .source_clk = UART_SCLK_DEFAULT,
     };
     uart_driver_install(UART_PORT_COMMAND, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
     uart_param_config(UART_PORT_COMMAND, &uart_config);
     uart_set_pin(UART_PORT_COMMAND, TXD_PIN_COMMAND, RXD_PIN_COMMAND, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
 }

  void tx_task(void *arg)
 {
     // Nhận thông điệp từ tham số truyền vào (arg)
     const char* message = (const char*) arg;
 
         while (1){// Gửi thông điệp qua UART
         uart_write_bytes(UART_PORT_COMMAND, message, strlen(message));
         vTaskDelay(5000);
         
        }
 }
 
void process_uart_command(const char* command) {
    // add write command
    if (strncmp(command, "write ", 6) == 0) {
        char* data = command + 6; // Skip "write " prefix
        
        if (strlen(data) > 0) {
            esp_err_t result = read_write(scanner, picc, data);
            if (result == ESP_OK) {
                const char* response = "Write successful\n";
                uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
            }
            else if (result == ESP_ERR_NOT_FOUND) {
                const char* response = "No card detected\n";
                uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
            }
            else {
                char response[50];
                snprintf(response, sizeof(response), "Write failed: %s\n", esp_err_to_name(result));
                uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
            }
        } else {
            const char* response = "Missing data to write\n";
            uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
        }
        uart_wait_tx_done(UART_PORT_COMMAND, pdMS_TO_TICKS(100));
    }
    else if (strcmp(command, "who") == 0) {
        const char* response = "Quyen's device\n";
        uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
        uart_wait_tx_done(UART_PORT_COMMAND, pdMS_TO_TICKS(100));
    } else if (strcmp(command, "uid") == 0) {
        char response[30];
        snprintf(response, sizeof(response), "UID: %s\n", g_uid);
        uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
        uart_wait_tx_done(UART_PORT_COMMAND, pdMS_TO_TICKS(100));
    } else if (strcmp(command, "atqa") == 0) {
        char response[30];
        snprintf(response, sizeof(response), "ATQA: %s\n", g_atqa);
        uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
        uart_wait_tx_done(UART_PORT_COMMAND, pdMS_TO_TICKS(100));
    } else if (strcmp(command, "sak") == 0) {
        char response[30];
        snprintf(response, sizeof(response), "SAK: %s\n", g_sak);
        uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
        uart_wait_tx_done(UART_PORT_COMMAND, pdMS_TO_TICKS(100));
    } else if (strcmp(command, "all")==0) {
        // send all information
        char response[100];
        snprintf(response, sizeof(response), "UID: %s\nATQA: %s\nSAK: %s\n", 
                g_uid, g_atqa, g_sak);
        uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
        uart_wait_tx_done(UART_PORT_COMMAND, pdMS_TO_TICKS(100));
    }
    else {
        const char* response = "Unknown command\n";
        uart_write_bytes(UART_PORT_COMMAND, response, strlen(response));
        uart_wait_tx_done(UART_PORT_COMMAND, pdMS_TO_TICKS(100));
    }
}

void rx_task(void *arg) {
    static const char *RX_TASK_TAG = "RX_TASK";
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE + 1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_PORT_COMMAND, data, RX_BUF_SIZE, 500);
        
        if (rxBytes > 0) {
            data[rxBytes] = '\0';
            printf("Received %d bytes: %s", rxBytes, data);
            process_uart_command((const char*)data);
        }
    }
    free(data);
}

void RFID_125_Task(void *arg) {
    printf("RDM6300 initialized. Waiting for tags...\n");
    while (1) {
       
       char data2[15] = {0};
        int len = uart_read_bytes(UART_PORT_RDM6300, data2, sizeof(data2) - 1, 100 / portTICK_PERIOD_MS);
        if (len > 0) {
            data2[len] = '\0';
            printf("RDM6300 Tag detected: %s\n", data2);
            ssd1306_clear_screen(&dev, false);
            ssd1306_contrast(&dev, 0xff);
            ssd1306_display_text(&dev, 0, data2, 15, false);
            if (is_mqtt_connected) {
                esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, data2, 0, 1, 0);
            }
            
        }
      
    }
}