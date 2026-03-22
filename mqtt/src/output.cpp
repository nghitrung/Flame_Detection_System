#include "output.h"
#include "global.h"

static bool blinkOnPhase() {
    return ((millis() / 1000UL) % 2UL) == 0UL;
}

void led_init(uint8_t pin_adc, uint8_t pin_led, int threshold){
    (void)pin_adc;
    (void)pin_led;
    (void)threshold;
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

void led_task(void* pv){
    (void)pv;
    led_init(0, 0, 0);
    while (1) {
        const uint32_t nowMs = millis();
        if (manual_alarm_on && manual_alarm_until_ms > 0 && nowMs > manual_alarm_until_ms) {
            manual_alarm_on = false;
            manual_alarm_until_ms = 0;
        }
        if (manual_emergency_on && manual_emergency_until_ms > 0 && nowMs > manual_emergency_until_ms) {
            manual_emergency_on = false;
            manual_emergency_until_ms = 0;
        }

        const bool autoTrigger = fire_alert && gas_alert && (glob_temperature > 45.0f);
        const bool trigger = autoTrigger || manual_alarm_on || manual_emergency_on;
        const bool led_state = trigger && blinkOnPhase();
        digitalWrite(LED_PIN, led_state ? HIGH : LOW);
        ledbuzzon = trigger;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void buzz_init(uint8_t pin_adc, uint8_t pin_buzz, int threshold){
    (void)pin_adc;
    (void)pin_buzz;
    (void)threshold;
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
}

void buzz_task(void* pv)
{
    (void)pv;
    buzz_init(0, 0, 0);
    while (1) {
        const uint32_t nowMs = millis();
        if (manual_alarm_on && manual_alarm_until_ms > 0 && nowMs > manual_alarm_until_ms) {
            manual_alarm_on = false;
            manual_alarm_until_ms = 0;
        }
        if (manual_emergency_on && manual_emergency_until_ms > 0 && nowMs > manual_emergency_until_ms) {
            manual_emergency_on = false;
            manual_emergency_until_ms = 0;
        }

        const bool autoTrigger = fire_alert && gas_alert && (glob_temperature > 45.0f);
        const bool trigger = autoTrigger || manual_alarm_on || manual_emergency_on;
        const bool buzz_state = trigger && blinkOnPhase();
        digitalWrite(BUZZER_PIN, buzz_state ? HIGH : LOW);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void button_task(void* pv) {
    (void)pv;
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    bool lastStableState = digitalRead(BUTTON_PIN);
    bool lastReading = lastStableState;
    uint32_t lastDebounceTime = millis();

    while (1) {
        const bool reading = digitalRead(BUTTON_PIN);
        const uint32_t nowMs = millis();

        if (reading != lastReading) {
            lastDebounceTime = nowMs;
            lastReading = reading;
        }

        if ((nowMs - lastDebounceTime) > 50UL && reading != lastStableState) {
            lastStableState = reading;

            // Button is active LOW when using INPUT_PULLUP.
            if (lastStableState == LOW) {
                const bool turnOn = !manual_emergency_on;
                manual_emergency_on = turnOn;
                manual_pump_on = turnOn;
                manual_alarm_on = turnOn;
                manual_emergency_until_ms = 0;
                manual_pump_until_ms = 0;
                manual_alarm_until_ms = 0;
                Serial.println(turnOn ? "[BUTTON] emergency toggled ON"
                                      : "[BUTTON] emergency toggled OFF");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}