#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"



#include "libs/include/task_connection.h"
#include "libs/include/task_co2.h"
#include "libs/include/task_inflamaveis.h"
#include "libs/include/task_display.h"
#include "libs/include/queue_sensores.h"

MQTT_CLIENT_DATA_T state_client; // Variável global para o estado do cliente MQTT
TaskHandle_t xTaskAlertaInflamaveis = NULL; // Handle da tarefa de alerta de inflamáveis
TaskHandle_t xTaskCO2 = NULL; // Handle da tarefa de CO2
QueueHandle_t xQueueCO2; // Fila de dados do sensor de gases CO2
QueueHandle_t xQueueInflamaveis; // Fila de dados do sensor de gases inflamáveis

bool controle_global_alarmes = true; // Variável global para controle de alarmes


int main()
{
    stdio_init_all();

    state_client.publish_mutex = xSemaphoreCreateMutex();

    xQueueCO2 = xQueueCreate(10, sizeof(float));
    xQueueInflamaveis = xQueueCreate(10, sizeof(float));

    xTaskCreate(vTaskConnection, "Task Connection", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskCO2, "Task CO2", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskInflamaveis, "Task Inflamaveis", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskAlertaInflamaveis, "Task Alerta Inflamaveis", configMINIMAL_STACK_SIZE, &state_client, 1, &xTaskAlertaInflamaveis);
    xTaskCreate(vTaskAlertaCO2, "Task Alerta CO2", configMINIMAL_STACK_SIZE, &state_client, 1, &xTaskCO2);
    xTaskCreate(vTaskDisplay, "Task Display", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    vTaskStartScheduler();
    panic_unsupported();
}
