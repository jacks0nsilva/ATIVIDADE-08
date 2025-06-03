#include <stdio.h>
#include "libs/include/task_display.h"
#include "libs/include/definicoes.h"
#include "libs/include/queue_sensores.h"
#include "task.h"

float co2 = 0.0, fumaca = 0.0;

void vTaskDisplay(void *params)
{
    while (true)
    {
        if(xQueueReceive(xQueueFumaca, &fumaca, portMAX_DELAY) == pdTRUE)
        {
            printf("Fumaça: %.0f ppm display\n", fumaca);
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    
}