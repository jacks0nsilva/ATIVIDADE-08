#ifndef CONNECTION_MQTT_H
#define CONNECTION_MQTT_H

#include "lwip/apps/mqtt.h"    
#include "lwip/apps/mqtt_priv.h" 
#include "lwip/dns.h"
#include "lwip/altcp_tls.h"
#include "definicoes.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
 
 //Dados do cliente MQTT
 typedef struct {
    mqtt_client_t* mqtt_client_inst;
    struct mqtt_connect_client_info_t mqtt_client_info;
    char data[MQTT_OUTPUT_RINGBUF_SIZE];
    char topic[MQTT_TOPIC_LEN];
    uint32_t len;
    ip_addr_t mqtt_server_address;
    bool connect_done;
    int subscribe_count;
    bool stop_client;
} MQTT_CLIENT_DATA_T;

 // Requisição para publicar
void pub_request_cb(__unused void *arg, err_t err);
 
 // Topico MQTT
 const char *full_topic(MQTT_CLIENT_DATA_T *state, const char *name);
 
 // Requisição de Assinatura - subscribe
 static void sub_request_cb(void *arg, err_t err);
 
 // Requisição para encerrar a assinatura
 static void unsub_request_cb(void *arg, err_t err);
 
 // Tópicos de assinatura
 static void sub_unsub_topics(MQTT_CLIENT_DATA_T* state, bool sub);
 
 // Dados de entrada MQTT
 static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
 
 // Dados de entrada publicados
 static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
 
 // Conexão MQTT
 static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
 
 // Inicializar o cliente MQTT
void start_client(MQTT_CLIENT_DATA_T *state);
 
 // Call back com o resultado do DNS
void dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg);

 #endif 