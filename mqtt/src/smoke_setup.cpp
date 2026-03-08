#include <MQGasKit.h>
#include "smoke_setup.h"
#include "global.h"

void vTaskSmoke(void *pvParameter) {
    MQGasKit mq2_1(SMOKE1_PIN, MQ2);
    MQGasKit mq2_2(SMOKE2_PIN, MQ2);  
    while (1) {
        if (xSensorMutex != NULL && xSemaphoreTake(xSensorMutex, portMAX_DELAY) == pdPASS) {
            smoke[0][0] = mq2_1.getPPM("CO")/ 10000.0;
            smoke[0][1] = mq2_1.getPPM("LPG") / 10000.0;
            smoke[0][2] = mq2_1.getPPM("Smoke") / 10000.0;

            smoke[1][0] = mq2_2.getPPM("CO") / 10000.0;
            smoke[1][1] = mq2_2.getPPM("LPG") / 10000.0;
            smoke[1][2] = mq2_2.getPPM("Smoke") / 10000.0;

            Serial.printf("\nCO1: %.2f | LPG1: %.2f | Smoke1: %.2f ppm", smoke[0][0], smoke[0][1], smoke[0][2]);
            Serial.printf("\nCO2 %.2f | LPG2: %.2f | Smoke2: %.2f ppm", smoke[1][0], smoke[1][1], smoke[1][2]);

            xSemaphoreGive(xSensorMutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(3000)); 
    }
}
