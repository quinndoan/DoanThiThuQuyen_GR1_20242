#ifndef FLASH_HANDLER_H
#define FLASH_HANDLER_H

#include "stdbool.h"
#include <stddef.h>
bool FlashHandler_getData(char *nameSpace, char *key, void *dataStore);
bool FlashHandler_setData(char *nameSpace, char *key, void *dataStore, size_t dataSize);


#endif