cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
set(FREERTOS_KERNEL_PATH "C:/FreeRTOS-Kernel")
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(ATIVIDADE-08 C CXX ASM) 
pico_sdk_init()


include_directories(${CMAKE_SOURCE_DIR}/libs/include) # Adiciona o diretório 'libs/include' à lista global de diretórios de headers 

add_executable(${PROJECT_NAME}  
        ATIVIDADE-08.c # Código principal em C
        libs/src/connection_mqtt.c #Código para comunicação MQTT
        libs/src/task_connection.c # Código da tarefa que mantém a conexão ativa
        libs/src/task_toxicos.c # Código da tarefa que lê o sensor MQ-135 (gases tóxicos) e publica no broker MQTT
        libs/src/task_inflamaveis.c # Código da tarefa que lê o sensor MQ-2 (gases inflamáveis) e publica no broker MQTT
        libs/src/task_display.c # Código da tarefa que exibe os dados no display LCD
        libs/src/ssd1306.c #Biblioteca para controle do display OLED SSD1306
        )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})  # Adiciona o diretório raiz como include privado apenas para o target atual

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    FreeRTOS-Kernel 
    FreeRTOS-Kernel-Heap4
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
    pico_mbedtls
    pico_lwip_mbedtls
    hardware_adc
    hardware_pwm
    hardware_clocks
    hardware_i2c
        )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)


pico_add_extra_outputs(${PROJECT_NAME})