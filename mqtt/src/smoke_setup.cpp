#include <MQGasKit.h>
#include "smoke_setup.h"
#include "global.h"

static float averagePpm(MQGasKit& sensor, const char* gasType) {
    const int samples = 5;
    float sum = 0.0f;
    int validCount = 0;
    for (int i = 0; i < samples; ++i) {
        float value = sensor.getPPM(gasType);
        if (!isnan(value) && isfinite(value) && value >= 0.0f && value <= 10000.0f) {
            sum += value;
            validCount++;
        }
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    return (validCount > 0) ? (sum / validCount) : NAN;
}

void vTaskSmoke(void *pvParameter) {
    MQGasKit mq2_1(SMOKE1_PIN, MQ2);
    MQGasKit mq2_2(SMOKE2_PIN, MQ2);  

    pinMode(SMOKE1_PIN, INPUT_PULLDOWN);
    pinMode(SMOKE2_PIN, INPUT_PULLDOWN);

    mq2_1.calibrate();
    mq2_2.calibrate();
    const uint32_t warmupStart = millis();

    while (1) {
        if (xSensorMutex != NULL && xSemaphoreTake(xSensorMutex, portMAX_DELAY) == pdPASS) {
            if (millis() - warmupStart < 30000UL) {
                smoke[0][0] = smoke[0][1] = smoke[0][2] = 0.0f;
                smoke[1][0] = smoke[1][1] = smoke[1][2] = 0.0f;
                gas_alert = false;
                Serial.print("\nMQ2 warmup... ignoring first 30s data");
                xSemaphoreGive(xSensorMutex);
                vTaskDelay(pdMS_TO_TICKS(3000));
                continue;
            }

            const int raw1 = analogRead(SMOKE1_PIN);
            const int raw2 = analogRead(SMOKE2_PIN);
            float smoke1 = averagePpm(mq2_1, "Smoke");
            float smoke2 = averagePpm(mq2_2, "Smoke");

            if (isnan(smoke1) || isnan(smoke2)) {
                Serial.print("\n Value is unreadable!");
                gas_alert = false;
            } else {
                // Input floating/disconnected often sits near ADC rails.
                if (raw1 <= 5 || raw1 >= 4090 || raw2 <= 5 || raw2 >= 4090) {
                    smoke[0][0] = smoke[0][1] = smoke[0][2] = 0.0f;
                    smoke[1][0] = smoke[1][1] = smoke[1][2] = 0.0f;
                    gas_alert = false;
                    Serial.printf("\nMQ2 input floating (raw1=%d raw2=%d), set gas=0", raw1, raw2);
                    xSemaphoreGive(xSensorMutex);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                    continue;
                }

                smoke[0][0] = averagePpm(mq2_1, "CO");
                smoke[0][1] = averagePpm(mq2_1, "LPG");
                smoke[0][2] = smoke1;

                smoke[1][0] = averagePpm(mq2_2, "CO");
                smoke[1][1] = averagePpm(mq2_2, "LPG");
                smoke[1][2] = smoke2;

                Serial.printf("\nCO1: %.2f | LPG1: %.2f | Smoke1: %.2f ppm", smoke[0][0], smoke[0][1], smoke[0][2]);
                Serial.printf("\nCO2 %.2f | LPG2: %.2f | Smoke2: %.2f ppm", smoke[1][0], smoke[1][1], smoke[1][2]);

                gas_alert = (smoke1 > SMOKE_THRESHOLD) || (smoke2 > SMOKE_THRESHOLD);
            }
            xSemaphoreGive(xSensorMutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(3000)); 
    }
}
