#include "libs/include/task_inflamaveis.h"
#include "libs/include/connection_mqtt.h"
#include "string.h"
#include "hardware/adc.h"
#include "semphr.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "queue_sensores.h"

bool alerta_inflamaveis_ativo = false;


void vTaskInflamaveis(void *params)
{
    adc_gpio_init(VRY_PIN); // Configura o pino do eixo Y do Joystick (Sensor MQ-2) como entrada analógica
    adc_init(); // Inicializa o ADC

    while(true)
    {
        adc_select_input(0); // Seleciona o canal ADC correspondente ao pino VRY_PIN
        uint16_t adc_value = adc_read(); // Lê o valor analógico do sensor MQ-2
        float ppm = (adc_value / 3500.0f) * LIMITE_INFLAMAVEIS; // Converte o valor ADC para ppm (partes por milhão)

        MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)params;

        // Verifica se a conexão MQTT foi estabelecida, se sim, publica o valor dos gases inflamáveis
        if(state->connect_done)
        {
            char str_ppm[10]; // Buffer para armazenar o valor de ppm como string
            sprintf(str_ppm, "%.0f", ppm);

            xSemaphoreTake(state->publish_mutex, portMAX_DELAY); // Pega o mutex para garantir acesso exclusivo à publicação
            mqtt_publish(state->mqtt_client_inst,full_topic(state, "/sensor/inflamaveis"), str_ppm, strlen(str_ppm), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
            xSemaphoreGive(state->publish_mutex);
            xQueueSend(xQueueInflamaveis, &ppm, portMAX_DELAY); // Envia o valor de ppm para a fila de alertas de inflamáveis
            ativar_alerta_inflamaveis(ppm, state); // Verifica se o valor de ppm excede o limite e ativa o alerta
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Aguarda 2 segundos antes da próxima leitura
    }
}


// Tarefa de alerta de inflamáveis
// Esta tarefa é responsável por ativar o buzzer e o LED quando o alerta de inflamáveis é acionado
void vTaskAlertaInflamaveis(void *params)
{
    gpio_set_function(BUZZER_A, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_A);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (5000 * 4096));  // Frequência do PWM
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(BUZZER_A, 0);  // Inicializa com o PWM desligado

    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Espera por uma notificação para ativar o alerta de inflamáveis

        // Enquanto o alerta de inflamáveis estiver ativo e o controle global de alarmes estiver habilitado, ativa o buzzer e o LED
        while(alerta_inflamaveis_ativo && controle_global_alarmes) {
            pwm_set_gpio_level(BUZZER_A, 2048);
            gpio_put(LED_RED, 1);

            vTaskDelay(pdMS_TO_TICKS(400));

            gpio_put(LED_RED, 0);
            pwm_set_gpio_level(BUZZER_A, 0);
            
            vTaskDelay(pdMS_TO_TICKS(400));
        }
    }
}


void ativar_alerta_inflamaveis(float ppm, MQTT_CLIENT_DATA_T *state){
    // Verifica se o valor de ppm excede o limite e se o alerta não está ativo
    if(ppm > LIMITE_INFLAMAVEIS && !alerta_inflamaveis_ativo && controle_global_alarmes) {
        alerta_inflamaveis_ativo = true; // Ativa o alerta de inflamáveis
        xTaskNotifyGive(xTaskAlertaInflamaveis); // Notifica a tarefa de alerta de inflamáveis para ativar o alerta
        publicar_alerta_inflamaveis(state); // Publica o alerta no tópico MQTT
    } else if (ppm <= LIMITE_INFLAMAVEIS && alerta_inflamaveis_ativo) {
        alerta_inflamaveis_ativo = false; // Desativa o alerta de inflamáveis
        publicar_alerta_inflamaveis(state); // Publica o estado do alerta no tópico MQTT
    }
}
 
// Publica o estado do alerta de inflamáveis no tópico MQTT
static void publicar_alerta_inflamaveis(void *params)
{
    char* str_msg = alerta_inflamaveis_ativo ? "On" : "Off";
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)params;
    xSemaphoreTake(state->publish_mutex, portMAX_DELAY);
    mqtt_publish(state->mqtt_client_inst,full_topic(state, "/alerta/inflamaveis"), str_msg, strlen(str_msg), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
    xSemaphoreGive(state->publish_mutex);
}