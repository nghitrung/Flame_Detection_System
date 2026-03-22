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
const uint32_t testDurationMs = 10000UL;
const uint32_t emergencyDurationMs = 20000UL;

static void applyControlAction(const String& action) {
    const uint32_t nowMs = millis();
    if (action == "pump_on") {
        manual_pump_on = true;
        manual_pump_until_ms = nowMs + testDurationMs;
        Serial.println("\n[CTRL] pump_on accepted");
    } else if (action == "pump_off") {
        manual_pump_on = false;
        manual_pump_until_ms = 0;
        Serial.println("\n[CTRL] pump_off accepted");
    } else if (action == "test_alarm") {
        manual_alarm_on = true;
        manual_alarm_until_ms = nowMs + testDurationMs;
        Serial.println("\n[CTRL] test_alarm accepted");
    } else if (action == "reset_system") {
        manual_pump_on = false;
        manual_alarm_on = false;
        manual_emergency_on = false;
        manual_pump_until_ms = 0;
        manual_alarm_until_ms = 0;
        manual_emergency_until_ms = 0;
        fire_alert = false;
        gas_alert = false;
        Serial.println("\n[CTRL] reset_system accepted");
    } else if (action == "full_test") {
        manual_pump_on = true;
        manual_alarm_on = true;
        manual_pump_until_ms = nowMs + testDurationMs;
        manual_alarm_until_ms = nowMs + testDurationMs;
        Serial.println("\n[CTRL] full_test accepted");
    } else if (action == "emergency_alert") {
        manual_emergency_on = true;
        manual_pump_on = true;
        manual_alarm_on = true;
        manual_emergency_until_ms = nowMs + emergencyDurationMs;
        manual_pump_until_ms = nowMs + emergencyDurationMs;
        manual_alarm_until_ms = nowMs + emergencyDurationMs;
        Serial.println("\n[CTRL] emergency_alert accepted");
    } else {
        Serial.print("\n[CTRL] unknown action: ");
        Serial.println(action);
    }
}

static void mqttCallback(char* topic, byte* payload, unsigned int length) {
    if (String(topic) != TOPIC_CONTROL) {
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
        Serial.print("\n[CTRL] invalid JSON: ");
        Serial.println(err.c_str());
        return;
    }

    const char* action = doc["action"];
    if (action == nullptr) {
        Serial.println("\n[CTRL] missing action field");
        return;
    }
    applyControlAction(String(action));
}

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
    } else if (String(topic) == TOPIC_DHT20) {
        JsonObject dht_obj = doc["dht20_data"].to<JsonObject>();
        dht_obj["TEMP"] = glob_temperature;
        dht_obj["HUMIDITY"] = glob_humidity;
    }
    serializeJson(doc, buffer);
    client.publish(topic, buffer);

    Serial.printf("\nPublished to %s: %s", topic, buffer);

    return String(buffer);
}

void vTaskMqtt(void* pvParameters) {
    bool log = false;
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);
    while (1) {
        client.loop();
        log = client.connected();

        if (log) {
            if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                Serial.print("\nMQTT still connected!");

                if (millis() - lastPublishTime >= publishInterval) {
                    lastPublishTime = millis();

                    packageData(TOPIC_SMOKE);

                    packageData(TOPIC_FLAME);

                    packageData(TOPIC_DHT20);

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
                        if (client.connect(clientID.c_str(), mqtt_user, mqtt_password)){
                            Serial.print("\nMQTT Server ");
                            Serial.print(mqtt_server);
                            Serial.print(":");
                            Serial.println(mqtt_port);
                            client.subscribe(TOPIC_CONTROL);
                            Serial.print("[CTRL] subscribed ");
                            Serial.println(TOPIC_CONTROL);
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