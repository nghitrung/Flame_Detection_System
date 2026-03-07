#include "wifi_setup.h"
#include "global.h"
#include "wifi_config.h"

void vTaskWifi(void* pvParameters) {
    while (1) {
        if (WiFi.status() != WL_CONNECTED) {
            if (xSemaphoreTake(xWifiMutex, portMAX_DELAY) == pdTRUE) {
                Serial.println("WiFi: Connecting...");
                WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

                int retry = 0;
                while (WiFi.status() != WL_CONNECTED && retry < 20) {
                    vTaskDelay(pdMS_TO_TICKS(500));
                    Serial.print(".");
                    retry++;
                }

                if (WiFi.status() == WL_CONNECTED) {
                    Serial.println("\nWiFi: Connected!");
                    Serial.print("IP: "); Serial.println(WiFi.localIP());
                }
                xSemaphoreGive(xWifiMutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // check after every 10 seconds
    }
}
