#ifndef TASK_TOXICOS_H
#define TASK_TOXICOS_H
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"

extern TaskHandle_t xTaskToxicos;
extern bool controle_global_alarmes;

void vTaskToxicos(void *params);
void ativar_alerta_toxicos(float ppm, MQTT_CLIENT_DATA_T *state);
void vTaskAlertaToxicos(void *params);
void publicar_alerta_toxicos(void *params);
#endif