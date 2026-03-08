#include <WiFi.h>
#include <PubSubClient.h>
#include "mqtt_setup.h"
#include "global.h"
#include "config.h"

// "172.24.168.18"
// "1883"

WiFiClient yoloClient;
PubSubClient client(yoloClient);

void mqtt_connect() {
    client.setServer(mqtt_server, 1883);
    
    uint32_t t = millis();

    while (!client.connected() && millis() - 1 < 5000) {
        if (client.connected()) {
            Serial.print("\nMQTT connected!");
        } else {
            Serial.print("\nMQTT connect time out ...");
        }
    }
    
}

void vTaskMqtt(void* pvParameters) {
    bool log = false;

    while (1) {
        Serial.print("\nMQTT: Connecting...");
        if (log) {
            if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                Serial.print("\nMQTT still connect!");
                xSemaphoreGive(xMqttMutex);
            } 
        } else {
            if (client.connected()) {
                if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                    Serial.print("\nMQTT Server"); Serial.println(mqtt_server);
                    String clientID = "YoloUno-" + String(random(0xffff), HEX);
                    if (client.connect(clientID.c_str())){
                        Serial.print("\nMQTT: Connected!");
                        client.subscribe(TOPIC_FLAME);
                    } else {
                        Serial.print("MQTT: Error = "); Serial.println(client.state());
                    }
                    xSemaphoreGive(xMqttMutex);
                }
                log = true;
            } else {
                log = false;
                if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                    Serial.print("\nMQTT is not connected!");
                    mqtt_connect();
                    xSemaphoreGive(xMqttMutex);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // MQTT beat
    }
}