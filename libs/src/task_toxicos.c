#include "libs/include/task_toxicos.h"
#include "libs/include/connection_mqtt.h"
#include "hardware/adc.h"
#include "string.h"
#include "semphr.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "libs/include/queue_sensores.h"

bool alerta_toxicos_ativo = false;

void vTaskToxicos(void *params)
{
    adc_gpio_init(VRX_PIN); // Configura o pino do eixo X do Joystick (Sensor MQ-135) como entrada analógica
    adc_init(); // Inicializa o ADC

    while(true)
    {
        adc_select_input(1); // Seleciona o canal ADC correspondente ao pino VRX_PIN
        uint16_t adc_value = adc_read(); // Lê o valor analógico do sensor MQ-135
        float ppm = (adc_value / 3500.0f) * LIMITE_TOXICOS; // Converte o valor ADC para ppm (partes por milhão)

        MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)params; // Obtém o estado do cliente MQTT

        // Verifica se a conexão MQTT foi estabelecida, se sim, publica o valor dos gases tóxicos
        if(state->connect_done)
        {
            char str_ppm[10]; // Buffer para armazenar o valor de ppm como string
            sprintf(str_ppm, "%.0f", ppm);

            xSemaphoreTake(state->publish_mutex, portMAX_DELAY); // Pega o mutex para garantir acesso exclusivo à publicação
            mqtt_publish(state->mqtt_client_inst,full_topic(state, "/sensor/toxicos"), str_ppm, strlen(str_ppm), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state); 
            xSemaphoreGive(state->publish_mutex);
            xQueueSend(xQueueToxicos, &ppm, portMAX_DELAY); // Envia o valor de ppm para a fila xQueueToxicos
            ativar_alerta_toxicos(ppm, state); // Verifica se o valor de ppm excede o limite e ativa o alerta
        } 
        vTaskDelay(pdMS_TO_TICKS(2000)); // Aguarda 2 segundos antes da próxima leitura
    }
}


// Tarefa de alerta de gases tóxicos
// Esta tarefa é responsável por ativar o buzzer e o LED quando o alerta  é acionado
void vTaskAlertaToxicos(void *params)
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
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Aguarda notificação para ativar o alerta 

        // Enquanto o alerta estiver ativo e o controle global de alarmes estiver habilitado, o buzzer e o LED serão ativados
        while(alerta_toxicos_ativo && controle_global_alarmes) {
            pwm_set_gpio_level(BUZZER_B, 2048);
            gpio_put(LED_BLUE, 1);

            vTaskDelay(pdMS_TO_TICKS(400));

            gpio_put(LED_BLUE, 0);
            pwm_set_gpio_level(BUZZER_B, 0);
            
            vTaskDelay(pdMS_TO_TICKS(400));
        }
    }
}



void ativar_alerta_toxicos(float ppm, MQTT_CLIENT_DATA_T *state){
    // Verifica se o valor de ppm excede o limite e se o alerta não está ativo
    if(ppm > LIMITE_TOXICOS && !alerta_toxicos_ativo && controle_global_alarmes) {
        alerta_toxicos_ativo = true; // Ativa o alerta de gases tóxicos
        xTaskNotifyGive(xTaskToxicos); // Notifica a tarefa de responsável pelo alerta de gases tóxicos
        publicar_alerta_toxicos(state); // Publica o alerta no tópico MQTT
    } else if (ppm <= LIMITE_TOXICOS && alerta_toxicos_ativo) {
        alerta_toxicos_ativo = false; // Desativa o alerta de gases tóxicos
        publicar_alerta_toxicos(state); // Publica o estado do alerta no tópico MQTT
    }
}

//// Função para publicar o estado do alerta no tópico MQTT
void publicar_alerta_toxicos(void *params)
{
    char* str_msg = alerta_toxicos_ativo ? "On" : "Off";
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)params;
    xSemaphoreTake(state->publish_mutex, portMAX_DELAY);
    mqtt_publish(state->mqtt_client_inst,full_topic(state, "/alerta/toxicos"), str_msg, strlen(str_msg), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
    xSemaphoreGive(state->publish_mutex);
}