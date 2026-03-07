#ifndef __MQTT_H__
#define __MQTT_H__

#include <global.h>

void mqtt_callback(char* topic, byte* payload, unsigned int length);
void vTaskMqtt(void* pvParameters);

#endif