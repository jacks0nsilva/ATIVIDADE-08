#include "libs/include/connection_mqtt.h"
#include "libs/include/task_connection.h"
#include "libs/include/definicoes.h"
#include "pico/cyw43_arch.h"
#include "pico/unique_id.h"


void vTaskConnection(void *params)
 {
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)params;
     INFO_printf("mqtt client starting\n");
 
     // Inicializa a arquitetura do cyw43
     if (cyw43_arch_init()) {
         panic("Failed to inizialize CYW43");
     }
 
     // Usa identificador único da placa
     char unique_id_buf[5];
     pico_get_unique_board_id_string(unique_id_buf, sizeof(unique_id_buf));
     for(int i=0; i < sizeof(unique_id_buf) - 1; i++) {
         unique_id_buf[i] = tolower(unique_id_buf[i]);
     }
 
     // Gera nome único, Ex: pico1234
     char client_id_buf[sizeof(MQTT_DEVICE_NAME) + sizeof(unique_id_buf) - 1];
     memcpy(&client_id_buf[0], MQTT_DEVICE_NAME, sizeof(MQTT_DEVICE_NAME) - 1);
     memcpy(&client_id_buf[sizeof(MQTT_DEVICE_NAME) - 1], unique_id_buf, sizeof(unique_id_buf) - 1);
     client_id_buf[sizeof(client_id_buf) - 1] = 0;
     INFO_printf("Device name %s\n", client_id_buf);
 
     state->mqtt_client_info.client_id = client_id_buf;
     state->mqtt_client_info.keep_alive = MQTT_KEEP_ALIVE_S; // Keep alive in sec
 #if defined(MQTT_USERNAME) && defined(MQTT_PASSWORD)
     state->mqtt_client_info.client_user = MQTT_USERNAME;
     state->mqtt_client_info.client_pass = MQTT_PASSWORD;
 #else
     state->mqtt_client_info.client_user = NULL;
     state->mqtt_client_info.client_pass = NULL;
 #endif
     static char will_topic[MQTT_TOPIC_LEN];
     strncpy(will_topic, full_topic(state, MQTT_WILL_TOPIC), sizeof(will_topic));
     state->mqtt_client_info.will_topic = will_topic;
     state->mqtt_client_info.will_msg = MQTT_WILL_MSG;
     state->mqtt_client_info.will_qos = MQTT_WILL_QOS;
     state->mqtt_client_info.will_retain = true;
 #if LWIP_ALTCP && LWIP_ALTCP_TLS
     // TLS enabled
 #ifdef MQTT_CERT_INC
     static const uint8_t ca_cert[] = TLS_ROOT_CERT;
     static const uint8_t client_key[] = TLS_CLIENT_KEY;
     static const uint8_t client_cert[] = TLS_CLIENT_CERT;
     // This confirms the indentity of the server and the client
     state->mqtt_client_info.tls_config = altcp_tls_create_config_client_2wayauth(ca_cert, sizeof(ca_cert),
             client_key, sizeof(client_key), NULL, 0, client_cert, sizeof(client_cert));
 #if ALTCP_MBEDTLS_AUTHMODE != MBEDTLS_SSL_VERIFY_REQUIRED
     WARN_printf("Warning: tls without verification is insecure\n");
 #endif
 #else
     state->client_info.tls_config = altcp_tls_create_config_client(NULL, 0);
     WARN_printf("Warning: tls without a certificate is insecure\n");
 #endif
 #endif
 
     // Conectar à rede WiFI - fazer um loop até que esteja conectado
     cyw43_arch_enable_sta_mode();
     if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
         panic("Failed to connect");
     }
     INFO_printf("\nConnected to Wifi\n");
 
     //Faz um pedido de DNS para o endereço IP do servidor MQTT
     cyw43_arch_lwip_begin();
     int err = dns_gethostbyname(MQTT_SERVER, &state->mqtt_server_address, dns_found, &state);
     cyw43_arch_lwip_end();
 
     // Se tiver o endereço, inicia o cliente
     if (err == ERR_OK) {
         start_client(state);
     } else if (err != ERR_INPROGRESS) { // ERR_INPROGRESS means expect a callback
         panic("dns request failed");
     }

    // Loop condicionado a conexão mqtt
    while (!state->connect_done || mqtt_client_is_connected(state->mqtt_client_inst)) {
    cyw43_arch_poll();
    cyw43_arch_wait_for_work_until(make_timeout_time_ms(10000));
    }
 }