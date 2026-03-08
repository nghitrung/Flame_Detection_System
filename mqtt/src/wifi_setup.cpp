#include "wifi_setup.h"
#include "global.h"
#include "config.h"

void vTaskWifi(void* pvParameters) {
    bool log = false;
    WiFi.begin(ssid, password);
    while (1) {
        if (log) {
            if (xWifiMutex != NULL && xSemaphoreTake(xWifiMutex, portMAX_DELAY) == pdPASS) {
                Serial.print("\nWifi still connected!");
                xSemaphoreGive(xWifiMutex);
            }
        } else {
            if (WiFi.status() == WL_CONNECTED ) {
                if (xWifiMutex != NULL && xSemaphoreTake(xWifiMutex, portMAX_DELAY) == pdPASS) {
                    Serial.print("\nWiFi: Connected!");
                    Serial.print("\nIP: "); 
                    Serial.println(WiFi.localIP());
                    xSemaphoreGive(xWifiMutex);
                }
                log = true;
            } else {
                log = false;
                if (xWifiMutex != NULL && xSemaphoreTake(xWifiMutex, portMAX_DELAY) == pdPASS) {
                    Serial.print("\nWifi is not connected!");
                    xSemaphoreGive(xWifiMutex);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500)); 
    } 
}
