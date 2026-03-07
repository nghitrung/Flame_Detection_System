#include <Arduino.h>
#include "global.h"
#include "wifi_setup.h"
#include "mqtt_setup.h"
#include "wifi_config.h"

void setup() {
    Serial.begin(115200);
    vTaskDelay(pdMS_TO_TICKS(1000)); 

    xWifiMutex = xSemaphoreCreateMutex();
    xMqttMutex = xSemaphoreCreateMutex();

    if (xWifiMutex != NULL && xMqttMutex != NULL) {
        xTaskCreate(vTaskWifi, "WiFi_Task", 4096, NULL, 1, NULL);
        
        xTaskCreate(vTaskMqtt, "MQTT_Task", 4096, NULL, 2, NULL);
        
        Serial.println("Hệ thống FreeRTOS đã khởi động...");
    } else {
        Serial.println("Lỗi khởi tạo Mutex!");
    }
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}