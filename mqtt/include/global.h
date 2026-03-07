#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t xMqttMutex;
extern SemaphoreHandle_t xWifiMutex;
extern SemaphoreHandle_t xSensorMutex;

#endif