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
        const bool trigger = fire_alert && gas_alert && (glob_temperature > 45.0f);
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
        const bool trigger = fire_alert && gas_alert && (glob_temperature > 45.0f);
        const bool buzz_state = trigger && blinkOnPhase();
        digitalWrite(BUZZER_PIN, buzz_state ? HIGH : LOW);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}