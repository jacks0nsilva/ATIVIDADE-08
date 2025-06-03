#ifndef TASK_INFLAMAVEIS_H
#define TASK_INFLAMAVEIS_H
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"

extern TaskHandle_t xTaskAlertaInflamaveis;
extern bool controle_global_alarmes;

void vTaskInflamaveis(void *params);
void ativar_alerta_inflamaveis(float ppm, MQTT_CLIENT_DATA_T *state);
void vTaskAlertaInflamaveis(void *params);
static void publicar_alerta_inflamaveis(void *params);
#endif