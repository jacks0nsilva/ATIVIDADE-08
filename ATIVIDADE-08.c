#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"



#include "libs/include/task_connection.h" // Tarefa de conexão MQTT
#include "libs/include/task_toxicos.h" // Tarefa de monitoramento de gases tóxicos
#include "libs/include/task_inflamaveis.h" // Tarefa de monitoramento de gases inflamáveis
#include "libs/include/task_display.h" // Tarefa de exibição de dados no display
#include "libs/include/queue_sensores.h" // Fila de dados dos sensores

MQTT_CLIENT_DATA_T state_client; // Variável global para o estado do cliente MQTT
TaskHandle_t xTaskAlertaInflamaveis = NULL; // Handle da tarefa de alerta de inflamáveis
TaskHandle_t xTaskToxicos = NULL; // Handle da tarefa dos gases tóxicos
QueueHandle_t xQueueToxicos; // Fila de dados do sensor de gases tóxicos
QueueHandle_t xQueueInflamaveis; // Fila de dados do sensor de gases inflamáveis

bool controle_global_alarmes = true; // Variável global para controle de alarmes


int main()
{
    stdio_init_all();

    state_client.publish_mutex = xSemaphoreCreateMutex();

    xQueueToxicos = xQueueCreate(10, sizeof(float));
    xQueueInflamaveis = xQueueCreate(10, sizeof(float));

    xTaskCreate(vTaskConnection, "Task Connection", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskToxicos, "Task Toxicos", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskInflamaveis, "Task Inflamaveis", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskAlertaInflamaveis, "Task Alerta Inflamaveis", configMINIMAL_STACK_SIZE, &state_client, 1, &xTaskAlertaInflamaveis);
    xTaskCreate(vTaskAlertaToxicos, "Task Alerta Toxicos", configMINIMAL_STACK_SIZE, &state_client, 1, &xTaskToxicos);
    xTaskCreate(vTaskDisplay, "Task Display", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    vTaskStartScheduler();
    panic_unsupported();
}
