#include "wifi_setup.h"
#include "global.h"
#include "config.h"

void wifi_connect() {
    WiFi.begin(ssid, password);

    uint32_t t = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - t < 5000) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("\nWifi connected!");
        } else {
            Serial.print("\nWifi connect time out ...");
        }
    }
}

void vTaskWifi(void* pvParameters) {
    bool log = false;

    while (1) {
        Serial.print("\nWiFi: Connecting...");
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
                    Serial.print("\nWifi is not connected, try to connect again");
                    wifi_connect();
                    xSemaphoreGive(xWifiMutex);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // check after every 10 seconds
    } 
}
