#include "pump.h"
#include "global.h"

void pump_control(void *pvParameters) {
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); 
  while (1) {
    int raw = analogRead(SMOKE1_PIN);
    for (int i = 0; i<3;i++)
    smoke[0][i] = raw;
    int raw = analogRead(SMOKE2_PIN);
    for (int j = 0; j<3;j++)
    smoke[1][j] = raw; 

    Serial.printf("smoke raw = %d, pump_on=%d\n", raw, pump_on ? 1 : 0);
    if (raw >= 2400) {
      if (pump_on) {
        Serial.println("[PUMP] Moisture high -> auto stop & latch OFF");
      }
      pump_on = false;          
      analogWrite(PUMP_PIN, 0);
    } else {
      if (pump_on) {
        analogWrite(PUMP_PIN, 250);   
      } else {
        analogWrite(PUMP_PIN, 0);    
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}