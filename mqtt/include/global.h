#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define SMOKE1_PIN A0
#define SMOKE2_PIN A1
#define FLAME1_DIGIPIN 17
#define FLAME1_ANAPIN 11
#define FLAME2_DIGIPIN 18
#define FLAME2_ANAPIN 12

#define PUMP_PIN   6
#define BUZZER_PIN 8
#define LED_PIN    10
//-------------DHT20+LCD
#define SDA_PIN   11
#define SCL_PIN   12
extern float smoke[2][3]; /*[{CO_PER1, LPG_PER1, SMOKE_PER1}
                             {CO_PER2, LPG_PER2, SMOKE_PER2}]*/ 
extern float flame[2]; // [Flame1, Flame2]
//DHT20_value
extern float glob_temperature;
extern float glob_humidity;
extern volatile bool pump_on;
extern volatile bool fire_alert;
extern volatile bool ledbuzzon;
extern volatile bool gas_alert;
extern const int SMOKE_THRESHOLD;

extern SemaphoreHandle_t xMqttMutex;
extern SemaphoreHandle_t xWifiMutex;
extern SemaphoreHandle_t xSensorMutex;
#endif