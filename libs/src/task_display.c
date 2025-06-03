#include <stdio.h>
#include "libs/include/task_display.h"
#include "libs/include/definicoes.h"
#include "libs/include/queue_sensores.h"
#include "task.h"
#include "hardware/i2c.h"
#include "libs/include/ssd1306.h"
#include <string.h>

float co2 = 0.0, fumaca = 0.0;

void vTaskDisplay(void *params)
{
    ssd1306_t ssd;

    char buffer_co2[10];
    char buffer_fumaca[10];

    // Inicialização da comunicação I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização do display e configuração
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    
    ssd1306_send_data(&ssd);
    while (true)
    {
        if(xQueueReceive(xQueueFumaca, &fumaca, portMAX_DELAY) == pdTRUE &&  xQueueReceive(xQueueCO2, &co2, portMAX_DELAY) == pdTRUE)
        {
            printf("CO2: %.0f ppm display\n", co2);
            ssd1306_fill(&ssd, false);
            ssd1306_rect(&ssd,0,0, WIDTH, HEIGHT, true, false); // Desenha o contorno do display

            ssd1306_draw_string(&ssd, "SENSORES", 34, 3); // Desenha o título
            ssd1306_hline(&ssd, 1,126, 16, true); // Linha horizontal abaixo do título;

            ssd1306_draw_string(&ssd, "CO2:", 4, 24);
            snprintf(buffer_co2, sizeof(buffer_co2), "%.0f ppm", co2);
            ssd1306_draw_string(&ssd, buffer_co2, 44, 24);
            ssd1306_hline(&ssd, 1,126, 40, true); // Linha horizontal abaixo do valor de CO2

            ssd1306_draw_string(&ssd, "Fumaca:", 4, 45);
            snprintf(buffer_fumaca, sizeof(buffer_fumaca), "%.0f ppm", fumaca);
            ssd1306_draw_string(&ssd, buffer_fumaca, 64, 45);

            ssd1306_send_data(&ssd); // Envia os dados para o display
        }
        else if(xQueueReceive(xQueueFumaca, &fumaca, portMAX_DELAY) == pdTRUE)
        {
            printf("Fumaça: %.0f ppm display\n", fumaca);
            ssd1306_fill(&ssd, false);
            ssd1306_rect(&ssd,0,0, WIDTH, HEIGHT, true, false); // Desenha o contorno do display

            ssd1306_draw_string(&ssd, "SENSORES", 34, 3); // Desenha o título
            ssd1306_hline(&ssd, 1,126, 16, true); // Linha horizontal abaixo do título;


            ssd1306_draw_string(&ssd, "Fumaca:", 4, 24);
            snprintf(buffer_fumaca, sizeof(buffer_fumaca), "%.0f ppm", fumaca);
            ssd1306_draw_string(&ssd, buffer_fumaca, 64, 24);
            ssd1306_hline(&ssd, 1,126, 40, true); // Linha horizontal abaixo do valor de fumaça

            ssd1306_draw_string(&ssd, "CO2:", 4, 45);
            sprintf(buffer_co2, "%.0f %s", co2, "ppm");
            ssd1306_draw_string(&ssd, buffer_co2, 44, 45); 

            ssd1306_send_data(&ssd); // Envia os dados para o display
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    
}