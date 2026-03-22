#include "output.h"
#include "global.h"
#include "smoke_setup.h"
#include "flame_setup.h"
#include "DHT+LCD.h"
void led_init(uint8_t pin_adc, uint8_t pin_buzz, int threshold){
        if ((gas_alert) && (fire_alert) && (glob_temperature > 45))
        {
            ledbuzzon = !ledbuzzon;
        }
}
void led_task(void* pv){
    if ((gas_alert) && (fire_alert) && (glob_temperature > 45))
        {
            ledbuzzon = !ledbuzzon;
        }
}
void buzz_init(uint8_t pin_adc, uint8_t pin_buzz, int threshold){
    if ((gas_alert) && (fire_alert) && (glob_temperature > 45))
        {
            ledbuzzon = !ledbuzzon;
        }
}
void buzz_task(void* pv)
{
    ledbuzzon = false;
        if ((gas_alert) && (fire_alert) && (glob_temperature > 45))
        {
            ledbuzzon = !ledbuzzon;
        }
}