#include "flame_setup.h"
#include "global.h"

void vTaskFlame(void *pvParameter) {
    float digi1 = digitalRead(FLAME1_DIGIPIN);
    float digi2 = digitalRead(FLAME2_DIGIPIN);
    float rawValue1 = analogRead(FLAME1_ANAPIN);
    float rawValue2 = analogRead(FLAME2_ANAPIN);

    while (1) {
        if (xSensorMutex != NULL && xSemaphoreTake(xSensorMutex, portMAX_DELAY) == pdPASS) {
            if (isnan(rawValue1) || isnan(rawValue2)) {
                Serial.print("\nValue is unreadable!"); 
            } else {
                if ((digi1 == HIGH) || (digi2 == HIGH))
                    fire_alert = true;
                else fire_alert = false;
                flame[0] = (1.0 - rawValue1/4095.0) * 100;
                flame[1] = (1.0 - rawValue2/4095.0) * 100;

                Serial.printf("\nRaw Value: %.2f | Flame Detect: %.2f", rawValue1, flame[0]); 
                Serial.printf("\nRaw Value: %.2f | Flame Detect: %.2f", rawValue2, flame[1]); 
            }
            xSemaphoreGive(xSensorMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}   
