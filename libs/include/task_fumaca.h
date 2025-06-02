#ifndef FUMACAO_H
#define FUMACAO_H
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"

extern TaskHandle_t xTaskAlertaFumaca;
extern bool controle_global_alarmes;

void vTaskFumaca(void *params);
void ativar_alerta_fumaca(float ppm, MQTT_CLIENT_DATA_T *state);
void vTaskAlertaFumaca(void *params);
static void publicar_alerta_fumaca(void *params);
#endif