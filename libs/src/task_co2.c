#include "libs/include/task_co2.h"
#include "libs/include/connection_mqtt.h"
#include "hardware/adc.h"
#include "string.h"
#include "semphr.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "libs/include/queue_sensores.h"

bool alerta_co2_ativo = false;

void vTaskCO2(void *params)
{
    adc_gpio_init(VRX_PIN); // Configura o pino do eixo X do Joystick (Sensor MQ-135) como entrada analógica
    adc_init(); // Inicializa o ADC

    while(true)
    {
        adc_select_input(1); // Seleciona o canal ADC correspondente ao pino VRX_PIN
        uint16_t adc_value = adc_read(); // Lê o valor analógico do sensor MQ-135
        float ppm = (adc_value / 4000.0f) * LIMITE_CO2; // Converte o valor ADC para ppm (partes por milhão)

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
            xQueueSend(xQueueCO2, &ppm, portMAX_DELAY); // Envia o valor de ppm para a fila xQueueCO2
            ativar_alerta_co2(ppm, state); // Verifica se o valor de ppm excede o limite e ativa o alerta
        } else {
            printf("aguardando conexão MQTT...\n");
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Aguarda 2 segundos antes da próxima leitura
    }
}

void vTaskAlertaCO2(void *params)
{
    gpio_set_function(BUZZER_B, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_B);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (5000 * 4096));  // Frequência do PWM
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(BUZZER_B, 0);  // Inicializa com o PWM desligado

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, 0);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        
        while(alerta_co2_ativo && controle_global_alarmes) {
            printf("Alerta de fumaça ativado!\n");
            pwm_set_gpio_level(BUZZER_B, 2048);
            gpio_put(LED_BLUE, 1);

            vTaskDelay(pdMS_TO_TICKS(400));

            gpio_put(LED_BLUE, 0);
            pwm_set_gpio_level(BUZZER_B, 0);
            
            vTaskDelay(pdMS_TO_TICKS(400));
        }
    }
}



void ativar_alerta_co2(float ppm, MQTT_CLIENT_DATA_T *state){
    if(ppm > LIMITE_CO2 && !alerta_co2_ativo && controle_global_alarmes) {
        alerta_co2_ativo = true; // Ativa o alerta de fumaça
        xTaskNotifyGive(xTaskCO2); // Notifica a tarefa de alerta de fumaça para ativar o alerta
        publicar_alerta_co2(state); // Publica o alerta no tópico MQTT
    } else if (ppm <= LIMITE_CO2 && alerta_co2_ativo) {
        alerta_co2_ativo = false; // Desativa o alerta de fumaça
        publicar_alerta_co2(state); // Publica o estado do alerta no tópico MQTT
    }
}

void publicar_alerta_co2(void *params)
{
    char* str_msg = alerta_co2_ativo ? "On" : "Off";
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)params;
    xSemaphoreTake(state->publish_mutex, portMAX_DELAY);
    mqtt_publish(state->mqtt_client_inst,full_topic(state, "/alerta/co2"), str_msg, strlen(str_msg), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
    xSemaphoreGive(state->publish_mutex);
}