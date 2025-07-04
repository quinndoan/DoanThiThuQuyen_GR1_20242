#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#define MQTT_TOPIC "rfid/tag"
#include "wifiHandler.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "Global.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_mac.h"
void mqtt_app_start();



#endif