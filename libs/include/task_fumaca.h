#ifndef FUMACAO_H
#define FUMACAO_H
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"

extern TaskHandle_t xTaskAlertaFumaca;

void vTaskFumaca(void *params);
void ativar_alerta(float ppm, MQTT_CLIENT_DATA_T *state);
void vTaskAlertaFumaca(void *params);
static void publicar_alerta(void *params);
#endif