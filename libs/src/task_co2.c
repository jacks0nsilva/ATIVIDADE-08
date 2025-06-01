#include "libs/include/task_co2.h"
#include "libs/include/connection_mqtt.h"
#include "hardware/adc.h"
#include "string.h"
#include "semphr.h"

void vTaskCO2(void *params)
{
    adc_gpio_init(VRX_PIN); // Configura o pino do eixo X do Joystick (Sensor MQ-135) como entrada analógica
    adc_init(); // Inicializa o ADC

    while(true)
    {
        adc_select_input(1); // Seleciona o canal ADC correspondente ao pino VRX_PIN
        uint16_t adc_value = adc_read(); // Lê o valor analógico do sensor MQ-135
        float ppm = (adc_value / 4095.0f) * LIMITE_CO2; // Converte o valor ADC para ppm (partes por milhão)

        printf("Valor de CO2: %.0f ppm\n", ppm);

        MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)params;
        if(state->connect_done)
        {
            // Publica o valor de CO2 no tópico MQTT
            char str_ppm[10];
            sprintf(str_ppm, "%.0f", ppm);
            xSemaphoreTake(state->publish_mutex, portMAX_DELAY);
            mqtt_publish(state->mqtt_client_inst,full_topic(state, "/sensor/co2"), str_ppm, strlen(str_ppm), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
            xSemaphoreGive(state->publish_mutex);
        } else {
            printf("aguardando conexão MQTT...\n");
        }



        vTaskDelay(pdMS_TO_TICKS(2000)); // Aguarda 2 segundos antes da próxima leitura
    }
}