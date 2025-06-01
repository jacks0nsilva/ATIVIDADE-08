#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libs/include/connection_mqtt.h"


#include "libs/include/task_connection.h"

MQTT_CLIENT_DATA_T state_client;

int main()
{
    stdio_init_all();

    xTaskCreate(vTaskConnection, "Task Connection", configMINIMAL_STACK_SIZE, &state_client, 1, NULL);
    vTaskStartScheduler();
    panic_unsupported();
}
