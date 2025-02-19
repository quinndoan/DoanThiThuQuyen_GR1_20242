#ifndef __RC522_INTERNAL_H__
#define __RC522_INTERNAL_H__

#include "rc522.h"


void rc522_task(void *arg);

esp_err_t rc522_dispatch_event(const rc522_handle_t rc522, rc522_event_t event, const void *data, size_t data_size);


#endif
