# 🏭 Estação de Monitoramento de Gases Industriais com MQTT e FreeRTOS

Este projeto simula uma estação de monitoramento ambiental voltada para uso industrial, utilizando a placa Raspberry Pi Pico W com FreeRTOS. Ele monitora os níveis de **CO₂** e **gases inflamáveis)**, simulados com um joystick, e envia os dados via **MQTT** para um broker. O sistema inclui alertas locais e visuais, com controle completo usando filas, semáforos e notificações de tarefas.

---

## 🎯 Objetivo

Criar uma estação IoT embarcada que:

- Monitore gases perigosos em tempo real.
- Utilize FreeRTOS com tarefas concorrentes, filas e notificações.
- Publique os dados via MQTT para integração com sistemas supervisórios.
- Acione alertas sonoros e visuais quando limites forem ultrapassados.
- Exiba os valores de forma local em um display OLED.
- Permita ativar/desativar alarmes remotamente via MQTT.

---

## ⚙️ Como Funciona

### 🎮 Simulação dos Sensores com Joystick

- **Eixo X (VRX)**: Simula o sensor **MQ-135 (CO₂)**
- **Eixo Y (VRY)**: Simula o sensor **MQ-2 (Gases inflamáveis)**

### 🔄 Tarefas com FreeRTOS

O sistema é dividido em várias tarefas principais:

1. `vTaskCO2()`:

   - Lê o eixo X.
   - Publica `/sensor/co2`.
   - Envia para `xQueueCO2`.

2. `vTaskInflamaveis()`:

   - Lê o eixo Y.
   - Publica `/sensor/inflamaveis`.
   - Envia para `xQueueInflamaveis`.

3. `vTaskAlertaCO2()` e `vTaskAlertaInflamaveis()`:

   - Disparam buzzer + LED quando ppm ultrapassa o limite estabelecido.
   - Publicam `/alerta/co2` e `/alerta/inflamaveis`.

4. `vTaskDisplay()`:

   - Recebe valores das filas.
   - Exibe no display OLED via protocolo I2C.

5. `vTaskConnection()`:
   - Conecta ao Wi-Fi.
   - Inicializa o cliente MQTT.
   - Mantém o polling do sistema.

---

## 🚨 Alertas Locais e Publicação MQTT

### 📢 Tópicos MQTT Publicados:

| Tópico                | Descrição                             |
| --------------------- | ------------------------------------- |
| `/sensor/co2`         | Valor simulado do sensor MQ-135 (ppm) |
| `/sensor/inflamaveis` | Valor simulado do sensor MQ-2 (ppm)   |
| `/alerta/co2`         | `"On"` ou `"Off"`                     |
| `/alerta/inflamaveis` | `"On"` ou `"Off"`                     |

---

## 📥 Tópicos MQTT Assinados:

| Tópico             | Descrição                                   |
| ------------------ | ------------------------------------------- |
| `/exit`            | Desconecta o cliente MQTT                   |
| `/alerta/controle` | Ativa/desativa todos os alarmes remotamente |

### 💡 Como usar `/alerta/controle`:

- Envie `"On"` para **desativar** os alarmes.
- Envie `"Off"` para **ativar** os alarmes novamente.

---

## 🧠 Condição de Alarme

- **CO₂ > 1000 ppm** ativa LED azul + buzzer B.
- **Gases inflamáveis > 500 ppm** ativa LED vermelho + buzzer A.
- Os alertas são desativados automaticamente quando os valores caem abaixo dos limites.

---

## 🧠 Sincronização com FreeRTOS

- **Filas (`xQueueCO2` e `xQueueInflamaveis`)**: Compartilham dados entre tarefas de sensor (emissor) e display (receptor).
- **Semáforo mutex**: Protege publicação MQTT simultânea.
- **Notificações de tarefa**: Ativam os alertas de forma eficiente.
- **Tarefa de conexão**: Mantém o cliente MQTT vivo em segundo plano.

---

## 🛠️ Como Executar o Projeto

### 1. Configure o ambiente

- SDK do Raspberry Pi Pico instalado.
- Extensão do VS Code para Pico configurada.
- FreeRTOS incluído no projeto.

### 2. Clone o repositório

```bash
git clone https://github.com/jacks0nsilva/ATIVIDADE-08.git
```

### 3. Ajuste o caminho do FreeRTOS no `CMakeLists.txt`:

```cmake
set(FREERTOS_KERNEL_PATH "/caminho/para/seu/FreeRTOS")
```

### 4. Altere as credenciais no arquivo `libs/include/definicoes.h`:

```c
#define WIFI_SSID "SUA REDE WIFI" // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASSWORD "SUA SENHA" // Substitua pela senha da sua rede Wi-Fi
#define MQTT_SERVER "10.0.0.6"  // Substitua pelo endereço do host - broket MQTT: Ex: 192.168.1.107
#define MQTT_USERNAME "admin"     // Substitua pelo nome da host MQTT - Username
#define MQTT_PASSWORD "admin"     // Substitua pelo Password da host MQTT - credencial de acesso - caso exista
```

### 4. Compile e execute

- **Clean CMake** → Limpeza do cache
- **Configure Project** → Detectar arquivos
- **Build Project** → Compilar e gerar `.uf2`
- **Run [USB]** → Gravar firmware na Pico W

---

## 📡 Requisitos Adicionais

- Broker MQTT local (ex: Mosquitto) ou online (ex: HiveMQ).

```

```
