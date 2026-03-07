#include <WiFi.h>
#include <PubSubClient.h>
#include "mqtt_setup.h"
#include "global.h"
#include "wifi_config.h"

WiFiClient espClient;
PubSubClient client(espClient);

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg = "";
    for (int i = 0; i < length; i++) msg += (char)payload[i];
    Serial.printf("MQTT Message [%s]: %s\n", topic, msg.c_str());
}

void vTaskMqtt(void* pvParameters) {
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(mqtt_callback);

    while (1) {
        if (WiFi.status() == WL_CONNECTED) {
            if (!client.connected()) {
                if (xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdTRUE) {
                    Serial.println("MQTT: Attempting connection...");
                    String clientId = "YoloUno-" + String(random(0xffff), HEX);
                    
                    if (client.connect(clientId.c_str())) {
                        Serial.println("MQTT: Connected!");
                        client.subscribe(TOPIC_ALARM);
                    } else {
                        Serial.print("MQTT: Failed, rc="); Serial.println(client.state());
                    }
                    xSemaphoreGive(xMqttMutex);
                }
            }

            if (client.connected()) {
                if (xSemaphoreTake(xMqttMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                    client.loop();
                    xSemaphoreGive(xMqttMutex);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // MQTT beat
    }
}