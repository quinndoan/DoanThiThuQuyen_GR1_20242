#include <esp_log.h>
#include "RIFD_Handler.h"
#include "picc/rc522_mifare.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
static const char *TAG = "rc522_reading_card";

extern char g_uid[20];
extern char g_atqa[10];
extern char g_sak[10];

rc522_driver_handle_t driver;
rc522_handle_t scanner;
rc522_spi_config_t driver_config = {
    .host_id = SPI3_HOST,
    .bus_config = &(spi_bus_config_t){
        .miso_io_num = RC522_SPI_BUS_GPIO_MISO,
        .mosi_io_num = RC522_SPI_BUS_GPIO_MOSI,
        .sclk_io_num = RC522_SPI_BUS_GPIO_SCLK,
    },
    .dev_config = {
        .spics_io_num = RC522_SPI_SCANNER_GPIO_SDA,
    },
    .rst_io_num = RC522_SCANNER_GPIO_RST,
};

// Biến lưu thẻ đang active
rc522_picc_t *active_picc = NULL;

// Hàm đọc liên tục thẻ RFID
void continuous_read_task(void *arg) {
    while (1) {
        if (active_picc != NULL) {
            // Đọc lại thông tin thẻ
            rc522_picc_t picc;
            esp_err_t ret = rc522_picc_print(&picc);
            
            if (ret == ESP_OK) {
                // Cập nhật thông tin thẻ
                *active_picc = picc;
                
                // Cập nhật các biến global
                char uid_buffer[RC522_PICC_UID_STR_BUFFER_SIZE_MAX];
                if (rc522_picc_uid_to_str(&picc.uid, uid_buffer, sizeof(uid_buffer)) == ESP_OK) {
                    strncpy(g_uid, uid_buffer, sizeof(g_uid) - 1);
                    g_uid[sizeof(g_uid) - 1] = '\0';
                }
                
                snprintf(g_atqa, sizeof(g_atqa), "%02X%02X", 
                         (uint8_t)((picc.atqa.source >> 8) & 0xFF),
                         (uint8_t)(picc.atqa.source & 0xFF));
                
                snprintf(g_sak, sizeof(g_sak), "%02X", picc.sak);
                
                ESP_LOGI(TAG, "Card continuously read - UID: %s, ATQA: %s, SAK: %s", 
                         g_uid, g_atqa, g_sak);
            } else {
                // Nếu không đọc được thẻ, có thể thẻ đã bị lấy ra
                active_picc = NULL;
                ESP_LOGI(TAG, "Card removed or not readable");
            }
        }
        
        // Đợi một khoảng thời gian ngắn trước khi đọc lại
        vTaskDelay((100));
    }
}

// Hàm chuyển đổi hex string thành bytes
esp_err_t hex_string_to_bytes(const char* hex_string, uint8_t* bytes, size_t max_len) {
    size_t len = strlen(hex_string);
    if (len % 2 != 0 || len / 2 > max_len) {
        return ESP_ERR_INVALID_ARG;
    }
    
    for (size_t i = 0; i < len; i += 2) {
        char byte_str[3] = {hex_string[i], hex_string[i+1], '\0'};
        bytes[i/2] = (uint8_t)strtol(byte_str, NULL, 16);
    }
    
    return ESP_OK;
}

// Hàm ghi dữ liệu vào thẻ RFID
esp_err_t write_to_rfid_card(const char* data) {
    esp_err_t ret = ESP_OK;
    
    // Kiểm tra xem có thẻ active không
    if (active_picc == NULL) {
        ESP_LOGE(TAG, "No active RFID card");
        return ESP_ERR_INVALID_STATE;
    }
    
    // Kiểm tra loại thẻ (Ultralight)
    if (active_picc->type != RC522_PICC_TYPE_MIFARE_UL) {
        ESP_LOGE(TAG, "Card is not MIFARE Ultralight");
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Writing data to MIFARE Ultralight card: %s", data);
    
    // Chuẩn bị dữ liệu để ghi
    size_t data_len = strlen(data);
    uint8_t block_data[RC522_MIFARE_BLOCK_SIZE] = {0};
    
    if (hex_string_to_bytes(data, block_data, RC522_MIFARE_BLOCK_SIZE) != ESP_OK) {
        ESP_LOGE(TAG, "Invalid hex data");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Ghi vào page 4 (vùng người dùng bắt đầu từ page 4)
    ret = rc522_mifare_write(scanner, active_picc, 4, block_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write data to RFID: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "Successfully wrote data to RFID card");
    
    // Cập nhật UID trong các biến global
    strncpy(g_uid, data, sizeof(g_uid) - 1);
    g_uid[sizeof(g_uid) - 1] = '\0';
    
    // Lưu data vào NVS
    save_rfid_data_to_nvs();
    
    return ESP_OK;
}

// Hàm khởi tạo NVS
esp_err_t init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

// Hàm lưu dữ liệu RFID vào NVS
void save_rfid_data_to_nvs(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Mở NVS handle
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return;
    }

    // Lưu dữ liệu
    err = nvs_set_str(nvs_handle, NVS_KEY_UID, g_uid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving UID: %s", esp_err_to_name(err));
    }

    err = nvs_set_str(nvs_handle, NVS_KEY_ATQA, g_atqa);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving ATQA: %s", esp_err_to_name(err));
    }

    err = nvs_set_str(nvs_handle, NVS_KEY_SAK, g_sak);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving SAK: %s", esp_err_to_name(err));
    }

    // Commit các thay đổi
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error committing NVS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "RFID data successfully saved to flash");
    }

    // Đóng NVS handle
    nvs_close(nvs_handle);
}

// Hàm đọc dữ liệu RFID từ NVS
void read_rfid_data_from_nvs(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    size_t required_size = 20;

    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "No saved RFID data found in NVS");
        return;
    }

    // Đọc UID
    err = nvs_get_str(nvs_handle, NVS_KEY_UID, g_uid, &required_size);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "No UID data found");
    } else {
        ESP_LOGI(TAG, "Loaded UID from NVS: %s", g_uid);
    }

    // Đọc ATQA
    required_size = 20;
    err = nvs_get_str(nvs_handle, NVS_KEY_ATQA, g_atqa, &required_size);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "No ATQA data found");
    } else {
        ESP_LOGI(TAG, "Loaded ATQA from NVS: %s", g_atqa);
    }

    // Đọc SAK
    required_size = 20;
    err = nvs_get_str(nvs_handle, NVS_KEY_SAK, g_sak, &required_size);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "No SAK data found");
    } else {
        ESP_LOGI(TAG, "Loaded SAK from NVS: %s", g_sak);
    }

    nvs_close(nvs_handle);
}

void on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    rc522_picc_state_changed_event_t *event = (rc522_picc_state_changed_event_t *)data;
    rc522_picc_t *picc = event->picc;

    if (picc->state == RC522_PICC_STATE_ACTIVE) {
        rc522_picc_print(picc);
        
        // Lưu trữ thẻ đang active
        active_picc = picc;
        
        // Lưu UID vào biến global
        char uid_buffer[RC522_PICC_UID_STR_BUFFER_SIZE_MAX];
        if (rc522_picc_uid_to_str(&picc->uid, uid_buffer, sizeof(uid_buffer)) == ESP_OK) {
            strncpy(g_uid, uid_buffer, sizeof(g_uid) - 1);
            g_uid[sizeof(g_uid) - 1] = '\0';
            ESP_LOGI(TAG, "Saved UID to global: %s", g_uid);
        }
        
        // Lưu ATQA vào biến global (chuyển thành chuỗi hex)
        snprintf(g_atqa, sizeof(g_atqa), "%02X%02X", 
                 (uint8_t)((picc->atqa.source >> 8) & 0xFF),
                 (uint8_t)(picc->atqa.source & 0xFF));
        ESP_LOGI(TAG, "Saved ATQA to global: %s", g_atqa);
        
        // Lưu SAK vào biến global (chuyển thành chuỗi hex)
        snprintf(g_sak, sizeof(g_sak), "%02X", picc->sak);
        ESP_LOGI(TAG, "Saved SAK to global: %s", g_sak);
        
        // Lưu dữ liệu vào NVS
        save_rfid_data_to_nvs();
    }
    else if (picc->state == RC522_PICC_STATE_IDLE && event->old_state >= RC522_PICC_STATE_ACTIVE) {
        ESP_LOGI(TAG, "Card has been removed");
        active_picc = NULL;
    }
}
