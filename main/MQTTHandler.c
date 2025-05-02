#include "MqttHandler.h"

#define MQTT_BROKER_URI "mqtt://broker.hivemq.com:1883"
esp_mqtt_client_handle_t mqtt_client = NULL;
static const char *TAG= "MQTT";
extern bool is_mqtt_connected;


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            is_mqtt_connected = true;
            ESP_LOGI(TAG, "MQTT connected");
            esp_mqtt_client_subscribe(mqtt_client, MQTT_TOPIC, 1);  
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT data received");
            printf("Topic: %.*s\n", event->topic_len, event->topic);
            printf("Data : %.*s\n", event->data_len, event->data);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT disconnected");
            break;

        default:
            break;
    }
}


void mqtt_app_start() {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}
