#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h>

void led_init(uint8_t pin_adc, uint8_t pin_buzz, int threshold);
void led_task(void* pv);
void buzz_init(uint8_t pin_adc, uint8_t pin_buzz, int threshold);
void buzz_task(void* pv);
#endif