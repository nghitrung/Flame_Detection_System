#include "pump.h"
#include "global.h"

void pump_control(void *pvParameters) {
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); 
  while (1) {
    int raw1 = analogRead(SMOKE1_PIN);
    for (int i = 0; i<3;i++)
    smoke[0][i] = raw1;
    int raw2 = analogRead(SMOKE2_PIN);
    for (int j = 0; j<3;j++)
    smoke[1][j] = raw2; 

    const bool trigger = fire_alert && gas_alert && (glob_temperature > 45.0f);
    pump_on = trigger;
    analogWrite(PUMP_PIN, trigger ? 250 : 0);

    Serial.printf("pump=%d fire=%d gas=%d temp=%.2f\n",
                  pump_on ? 1 : 0, fire_alert ? 1 : 0, gas_alert ? 1 : 0, glob_temperature);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}