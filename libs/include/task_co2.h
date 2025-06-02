#ifndef TASK_CO2_H
#define TASK_CO2_H
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"

extern TaskHandle_t xTaskCO2;

void vTaskCO2(void *params);
void ativar_alerta_co2(float ppm, MQTT_CLIENT_DATA_T *state);
void vTaskAlertaCO2(void *params);
void publicar_alerta_co2(void *params);
#endif