#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"


#include "libs/include/task_connection.h"
#include "libs/include/task_co2.h"
#include "libs/include/task_fumaca.h"

MQTT_CLIENT_DATA_T state_client; // Variável global para o estado do cliente MQTT

int main()
{
    stdio_init_all();

    state_client.publish_mutex = xSemaphoreCreateMutex();
    xTaskCreate(vTaskConnection, "Task Connection", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskCO2, "Task CO2", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    xTaskCreate(vTaskFumaca, "Task Fumaca", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    vTaskStartScheduler();
    panic_unsupported();
}
