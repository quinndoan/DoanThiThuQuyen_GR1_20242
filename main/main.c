#include <esp_log.h>
//#include "rc522.h"
#include "rc522_driver.h" // Ensure this header is included
#include "rc522_spi.h"
#include "rc522_picc.h"
#include "rc522_internal.h"
#include "rc522_driver_internal.h"
#include "rc522_types.h"
#include "rc522_picc_internal.h"
#include "rc522_picc.h"
#include "rc522_types_internal.h"
#include "rc522_types.h"
#include "rc522_pcd.h"
#include "rc522_pcd_internal.h"
#include "rc522_helpers_internal.h"

#include "task_common.h"
#include "FlashHandler.h"
#include <strings.h>

esp_err_t rc522_create(const rc522_config_t *config, rc522_handle_t *out_rc522);

esp_err_t rc522_register_events(
    const rc522_handle_t rc522, rc522_event_t event, esp_event_handler_t event_handler, void *event_handler_arg);

esp_err_t rc522_unregister_events(const rc522_handle_t rc522, rc522_event_t event, esp_event_handler_t event_handler);

esp_err_t rc522_start(rc522_handle_t rc522);

esp_err_t rc522_pause(rc522_handle_t rc522);

esp_err_t rc522_destroy(rc522_handle_t rc522);

static const char *TAG = "rc522_reading_card";

static char uid_tem[20];

#define RC522_SPI_BUS_GPIO_MISO    (19)
#define RC522_SPI_BUS_GPIO_MOSI    (21)
#define RC522_SPI_BUS_GPIO_SCLK    (22)
#define RC522_SPI_SCANNER_GPIO_SDA (23)
#define RC522_SCANNER_GPIO_RST     (-1) // soft-reset

static rc522_spi_config_t driver_config = {
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

static rc522_driver_handle_t driver;
static rc522_handle_t scanner;

static void on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    rc522_picc_state_changed_event_t *event = (rc522_picc_state_changed_event_t *)data;
    rc522_picc_t *picc = event->picc;

    if (picc->state == RC522_PICC_STATE_ACTIVE) {
        rc522_picc_print(picc);

        // Check if UID is already stored
        rc522_picc_t stored_picc;

        if (FlashHandler_getData(NAMESPACE_DEVICE_INFO, KEY_UID, &uid_tem) != ESP_OK) { // phai chay thu lai xem co dung logic khong, neu sai sua cho uid_tem phai khac DEFAULT
        // UID not found, save the card info
            bzero(g_uid, sizeof(g_uid));
            bzero(g_type, sizeof(g_type));   
            bzero(g_atqa, sizeof(g_atqa));
            bzero(g_sak, sizeof(g_sak));

            if (FlashHandler_setData(NAMESPACE_DEVICE_INFO, KEY_UID, &g_uid, sizeof(g_uid)) &&
                FlashHandler_setData(NAMESPACE_DEVICE_INFO, KEY_TYPE, &g_type, sizeof(g_type)) &&
                FlashHandler_setData(NAMESPACE_DEVICE_INFO, KEY_ATQA, &g_atqa, sizeof(g_atqa)) &&
                FlashHandler_setData(NAMESPACE_DEVICE_INFO, KEY_SAK, &g_sak, sizeof(g_sak)))
                {
                ESP_LOGI(TAG, "Card info saved to flash");
                }
            }
            else{
                ESP_LOGI(TAG, "Error when saving info to flash or device already saved");
            }
    }
    else if (picc->state == RC522_PICC_STATE_IDLE && event->old_state >= RC522_PICC_STATE_ACTIVE) {
        ESP_LOGI(TAG, "Card has been removed");
    }
}

void app_main()
{
    rc522_spi_create(&driver_config, &driver);
    rc522_driver_install(driver);

    rc522_config_t scanner_config = {
        .driver = driver,
    };

    rc522_create(&scanner_config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_picc_state_changed, NULL); // handle variable, trigger, handle function, null
    rc522_start(scanner);
}
