#include "global.h"

float smoke[2][3] = {{0, 0, 0}, {0, 0, 0}}; /*[{CO_PER1, LPG_PER1, SMOKE_PER1}
                                               {CO_PER2, LPG_PER2, SMOKE_PER2}]*/ 
float flame[2] = {0, 0}; // [flame1, flame2]
//dht20
float glob_temperature = 0.0f;
float glob_humidity = 0.0f;
SemaphoreHandle_t xMqttMutex = NULL;
SemaphoreHandle_t xWifiMutex = NULL;
SemaphoreHandle_t xSensorMutex = NULL;
volatile bool pump_on = false;
volatile bool fire_alert = false;
volatile bool ledbuzzon = false;
volatile bool gas_alert = false;
const int SMOKE_THRESHOLD = 1200;