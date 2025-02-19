#ifndef __RC522_DRIVER_H__
#define __RC522_DRIVER_H__

#include "rc522_types.h"


typedef struct rc522_driver_handle *rc522_driver_handle_t;

esp_err_t rc522_driver_install(const rc522_driver_handle_t driver);

esp_err_t rc522_driver_uninstall(const rc522_driver_handle_t driver);


#endif