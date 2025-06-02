#include "libs/include/task_fumaca.h"
#include "libs/include/connection_mqtt.h"
#include "string.h"
#include "hardware/adc.h"
#include "semphr.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

bool alerta_fumaca_ativo = false;


void vTaskFumaca(void *params)
{
    adc_gpio_init(VRY_PIN); // Configura o pino do eixo Y do Joystick (Sensor MQ-2) como entrada analógica
    adc_init(); // Inicializa o ADC

    while(true)
    {
        adc_select_input(0); // Seleciona o canal ADC correspondente ao pino VRY_PIN
        uint16_t adc_value = adc_read(); // Lê o valor analógico do sensor MQ-2
        float ppm = (adc_value / 4000.0f) * LIMITE_FUMACA; // Converte o valor ADC para ppm (partes por milhão)

        printf("Valor de fumaça: %.0f ppm\n", ppm);

        MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)params;
        if(state->connect_done)
        {
            // Publica o valor de CO2 no tópico MQTT
            char str_ppm[10];
            sprintf(str_ppm, "%.0f", ppm);
            xSemaphoreTake(state->publish_mutex, portMAX_DELAY);
            mqtt_publish(state->mqtt_client_inst,full_topic(state, "/sensor/fumaca"), str_ppm, strlen(str_ppm), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
            xSemaphoreGive(state->publish_mutex);
            ativar_alerta_fumaca(ppm, state); // Verifica se o valor de ppm excede o limite e ativa o alerta
        } else {
            printf("aguardando conexão MQTT...\n");
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Aguarda 2 segundos antes da próxima leitura
    }
}

void vTaskAlertaFumaca(void *params)
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
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        
        while(alerta_fumaca_ativo && controle_global_alarmes) {
            printf("Alerta de fumaça ativado!\n");
            pwm_set_gpio_level(BUZZER_A, 2048);
            gpio_put(LED_RED, 1);

            vTaskDelay(pdMS_TO_TICKS(400));

            gpio_put(LED_RED, 0);
            pwm_set_gpio_level(BUZZER_A, 0);
            
            vTaskDelay(pdMS_TO_TICKS(400));
        }
    }
}


void ativar_alerta_fumaca(float ppm, MQTT_CLIENT_DATA_T *state){
    // Verifica se o valor de ppm excede o limite e se o alerta não está ativo
    if(ppm > LIMITE_FUMACA && !alerta_fumaca_ativo && controle_global_alarmes) {
        alerta_fumaca_ativo = true; // Ativa o alerta de fumaça
        xTaskNotifyGive(xTaskAlertaFumaca); // Notifica a tarefa de alerta de fumaça para ativar o alerta
        publicar_alerta_fumaca(state); // Publica o alerta no tópico MQTT
    } else if (ppm <= LIMITE_FUMACA && alerta_fumaca_ativo) {
        alerta_fumaca_ativo = false; // Desativa o alerta de fumaça
        publicar_alerta_fumaca(state); // Publica o estado do alerta no tópico MQTT
    }
}
 
static void publicar_alerta_fumaca(void *params)
{
    char* str_msg = alerta_fumaca_ativo ? "On" : "Off";
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)params;
    xSemaphoreTake(state->publish_mutex, portMAX_DELAY);
    mqtt_publish(state->mqtt_client_inst,full_topic(state, "/alerta/fumaca"), str_msg, strlen(str_msg), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
    xSemaphoreGive(state->publish_mutex);
}