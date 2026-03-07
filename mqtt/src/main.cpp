#include <Arduino.h>
#include "global.h"
#include "wifi_setup.h"
#include "mqtt_setup.h"
#include "wifi_config.h"

void setup() {
    Serial.begin(115200);
    unsigned long start = millis();
    while (!Serial && (millis() - start < 3000)); 

    Serial.println("\n=== HE THONG KHOI DONG ===");

    xWifiMutex = xSemaphoreCreateMutex();
    xMqttMutex = xSemaphoreCreateMutex();

    if (xWifiMutex != NULL && xMqttMutex != NULL) {
        xTaskCreate(vTaskWifi, "WiFi_Task", 4096, NULL, 1, NULL);
        
        xTaskCreate(vTaskMqtt, "MQTT_Task", 4096, NULL, 2, NULL);
        
        Serial.println("System is working!");
    } else {
        Serial.println("Failed to create mutex!");
    }
}

void loop() {
    return;
}