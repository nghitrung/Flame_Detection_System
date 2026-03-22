#include "pump.h"
#include "global.h"

void pump_control(void *pvParameters) {
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); 
  while (1) {
    const uint32_t nowMs = millis();
    if (manual_pump_on && manual_pump_until_ms > 0 && nowMs > manual_pump_until_ms) {
      manual_pump_on = false;
      manual_pump_until_ms = 0;
    }
    if (manual_emergency_on && manual_emergency_until_ms > 0 && nowMs > manual_emergency_until_ms) {
      manual_emergency_on = false;
      manual_emergency_until_ms = 0;
    }

    const bool autoTrigger = fire_alert && gas_alert && (glob_temperature > 45.0f);
    const bool manualTrigger = manual_pump_on || manual_emergency_on;
    const bool trigger = autoTrigger || manualTrigger;
    pump_on = trigger;
    analogWrite(PUMP_PIN, trigger ? 250 : 0);

    Serial.printf("pump=%d fire=%d gas=%d temp=%.2f manual=%d\n",
                  pump_on ? 1 : 0, fire_alert ? 1 : 0, gas_alert ? 1 : 0,
                  glob_temperature, manualTrigger ? 1 : 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}