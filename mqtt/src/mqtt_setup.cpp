#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "mqtt_setup.h"
#include "global.h"
#include "config.h"

WiFiClient yoloClient;
PubSubClient client(yoloClient);
long lastPublishTime = 0;
const long publishInterval = 3000;

String packageData(const char* topic) {
    JsonDocument doc;
    char buffer[256];

    if (String(topic) == TOPIC_SMOKE) {
        JsonObject mq2_1_obj = doc["mq2_1"].to<JsonObject>();
        mq2_1_obj["CO"] = smoke[0][0];
        mq2_1_obj["LPG"] = smoke[0][1];
        mq2_1_obj["SMOKE"] = smoke[0][2];

        JsonObject mq2_2_obj = doc["mq2_2"].to<JsonObject>();
        mq2_2_obj["CO"] = smoke[1][0];
        mq2_2_obj["LPG"] = smoke[1][1];
        mq2_2_obj["SMOKE"] = smoke[1][2];
    } else if (String(topic) == TOPIC_FLAME) {
        JsonObject flame_obj = doc["flame_data"].to<JsonObject>();
        flame_obj["FLAME1"] = flame[0];
        flame_obj["FLAME2"] = flame[1];
    }
    serializeJson(doc, buffer);
    client.publish(topic, buffer);

    Serial.printf("\nPublished to %s: %s", topic, buffer);

    return String(buffer);
}

void vTaskMqtt(void* pvParameters) {
    bool log = false;
    client.setServer(mqtt_server, 1883);
    while (1) {
        if (log) {
            if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                Serial.print("\nMQTT still connected!");

                if (millis() - lastPublishTime >= publishInterval) {
                    lastPublishTime = millis();

                    packageData(TOPIC_SMOKE);

                    packageData(TOPIC_FLAME);

                    Serial.print("\n Data is sent!");
                }

                xSemaphoreGive(xMqttMutex);
            }
        } else {
            if (WiFi.status() == WL_CONNECTED) {
                if (!client.connected()) {
                    if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                        Serial.print("\nMQTT: Connecting...");
                        String clientID = "Sensor-" + String(random(0xffff), HEX);
                        if (client.connect(clientID.c_str())){
                            Serial.print("\nMQTT Server"); Serial.println(mqtt_server);
                            log = true;
                        } else {
                            Serial.print("\nMQTT: Error = "); Serial.println(client.state());
                            log = false;
                        }
                        xSemaphoreGive(xMqttMutex);
                    } else {
                        Serial.print("\nWaiting mutex ...");
                        log = false;
                    }
                } 
            } else {
                Serial.print("\nWaiting WIFI ...");
                log = false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // MQTT beat
    }
}