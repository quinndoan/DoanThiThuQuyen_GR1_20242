#ifndef RC522_I2C_H
#define RC522_I2C_H
#include "i2c.h"
#include "driver/gpio.h"
#include "rc522_driver.h"


typedef struct
{
    i2c_config_t config;
    i2c_port_t port;
    uint8_t device_address;
    uint32_t rw_timeout_ms;

    /**
     * GPIO number of the RC522 RST pin.
     * Set to -1 if the RST pin is not connected.
     */
    gpio_num_t rst_io_num;
} rc522_i2c_config_t;

esp_err_t rc522_i2c_create(const rc522_i2c_config_t *config, rc522_driver_handle_t *driver);


#endif