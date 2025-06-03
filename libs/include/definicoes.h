#ifndef DEBUG_printf
#ifndef NDEBUG
#define DEBUG_printf printf
#else
#define DEBUG_printf(...)
#endif
#endif

#ifndef INFO_printf
#define INFO_printf printf
#endif

#ifndef ERROR_printf
#define ERROR_printf printf
#endif


// Manter o programa ativo - keep alive in seconds
#define MQTT_KEEP_ALIVE_S 60

// QoS - mqtt_subscribe
// At most once (QoS 0)
// At least once (QoS 1)
// Exactly once (QoS 2)
#define MQTT_SUBSCRIBE_QOS 1
#define MQTT_PUBLISH_QOS 1
#define MQTT_PUBLISH_RETAIN 0

// Tópico usado para: last will and testament
#define MQTT_WILL_TOPIC "/online"
#define MQTT_WILL_MSG "0"
#define MQTT_WILL_QOS 1

#ifndef MQTT_DEVICE_NAME
#define MQTT_DEVICE_NAME "pico"
#endif

// Definir como 1 para adicionar o nome do cliente aos tópicos, para suportar vários dispositivos que utilizam o mesmo servidor
#ifndef MQTT_UNIQUE_TOPIC
#define MQTT_UNIQUE_TOPIC 0
#endif

//#define WIFI_SSID "SUA REDE WIFI"                  // Substitua pelo nome da sua rede Wi-Fi
//#define WIFI_PASSWORD "SUA SENHA"      // Substitua pela senha da sua rede Wi-Fi
#define MQTT_SERVER "10.0.0.6"                // Substitua pelo endereço do host - broket MQTT: Ex: 192.168.1.107
#define MQTT_USERNAME "admin"     // Substitua pelo nome da host MQTT - Username
#define MQTT_PASSWORD "admin"     // Substitua pelo Password da host MQTT - credencial de acesso - caso exista



#ifndef MQTT_SERVER
#error Need to define MQTT_SERVER
#endif

// This file includes your client certificate for client server authentication
#ifdef MQTT_CERT_INC
#include MQTT_CERT_INC
#endif

#ifndef MQTT_TOPIC_LEN
#define MQTT_TOPIC_LEN 100
#endif

/* DEFINIÇÕES DOS PERIFÉRICOS */

#define VRX_PIN 27 // Eixo X do Joystick (Sensor MQ-135)
#define VRY_PIN 26 // Eixo Y do Joystick (Sensor MQ-2)
#define BUZZER_A 21 // Pino do Buzzer
#define BUZZER_B 10 // Pino do Buzzer
#define LED_RED 13 // Pino do LED Vermelho
#define LED_BLUE 12 // Pino do LED Azul
#define I2C_PORT i2c1 // Porta I2C
#define I2C_SDA 14 // Pino SDA
#define I2C_SCL 15 // Pino SCL
#define ADRESS 0x3c // Endereço do display

/*DEFINIÇÕES PARA CÁLCULOS*/

#define LIMITE_CO2 1000 // Limite de CO2 em ppm (partes por milhão)
#define LIMITE_FUMACA 500 // Limite de fumaça em ppm (partes por milhão)