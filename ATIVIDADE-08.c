#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"



#include "libs/include/task_connection.h"
#include "libs/include/task_co2.h"
#include "libs/include/task_fumaca.h"
#include "libs/include/task_display.h"
#include "libs/include/queue_sensores.h"

MQTT_CLIENT_DATA_T state_client; // Variável global para o estado do cliente MQTT
TaskHandle_t xTaskAlertaFumaca = NULL; // Handle da tarefa de alerta de fumaça
TaskHandle_t xTaskCO2 = NULL; // Handle da tarefa de CO2
QueueHandle_t xQueueCO2; // Fila para alertas de fumaça
QueueHandle_t xQueueFumaca; // Fila para alertas de CO2

bool controle_global_alarmes = true; // Variável global para controle de alarmes


int main()
{
    stdio_init_all();

    state_client.publish_mutex = xSemaphoreCreateMutex();

    xQueueCO2 = xQueueCreate(10, sizeof(float));
    xQueueFumaca = xQueueCreate(10, sizeof(float));

    xTaskCreate(vTaskConnection, "Task Connection", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskCO2, "Task CO2", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskFumaca, "Task Fumaca", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskAlertaFumaca, "Task Alerta Fumaca", configMINIMAL_STACK_SIZE, &state_client, 1, &xTaskAlertaFumaca);
    xTaskCreate(vTaskAlertaCO2, "Task Alerta CO2", configMINIMAL_STACK_SIZE, &state_client, 1, &xTaskCO2);
    xTaskCreate(vTaskDisplay, "Task Display", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    vTaskStartScheduler();
    panic_unsupported();
}
