
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "FlashHandler.h"

#define log_info(format, ...)
#define log_err(format, ...)

bool FlashHandler_getData(char *nameSpace, char *key, void *dataStore)
{
    log_info("Getting data store in flash");
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(nameSpace, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        log_err("open nvs error");
        return false;
    }

    // Read the size of memory space required for blob
    size_t required_size = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_blob(my_handle, key, NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
    {
        log_err("nvs_get_blob error");
        return false;
    }

    // Read previously saved blob if available
    if (required_size > 0)
    {
        log_info("Found data store in flash");
        err = nvs_get_blob(my_handle, key, dataStore, &required_size);
        if (err != ESP_OK)
        {
            log_err("nvs_get_blob error");
            return false;
        }
    }

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        log_err("nvs_commit error");
        return false;
    }
    // Close
    nvs_close(my_handle);
    log_info("Get data store in flash done");
    if (required_size > 0)
    {
        return true;
    }

    return false;
}
bool FlashHandler_setData(char *nameSpace, char *key, void *dataStore, size_t dataSize)
{
    log_info("Saving data to flash");
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(nameSpace, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        log_err("open nvs error");
        return false;
    }

    // Read the size of memory space required for blob
    size_t required_size = 0; // value will default to 0, if not set yet in NVS
    // Write value including previously saved blob if available
    required_size = dataSize;
    log_info("store data size: %d", required_size);
    err = nvs_set_blob(my_handle, key, dataStore, required_size);

    if (err != ESP_OK)
    {
        log_err("nvs_set_blob error %d", err);
        return false;
    }

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        log_err("nvs_commit error");
        return false;
    }
    // Close
    nvs_close(my_handle);
    log_info("Data is saved\n");
    return true;
}