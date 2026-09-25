# 🏎️ 4WD JuaMaker - Robô Controlado via Web (ESP32)

## 🎯 Objetivo do Projeto
O **4WD JuaMaker** é um veículo robótico de tração nas 4 rodas (4WD) controlado por uma interface web intuitiva e moderna, hospedada diretamente no próprio microcontrolador (ESP32). 

O projeto foi criado com o objetivo de demonstrar a integração entre hardware de controle de motores, conectividade Wi-Fi autônoma (Modo Access Point) e desenvolvimento web assíncrono moderno, eliminando a necessidade de aplicativos de terceiros ou roteadores externos para operar o robô.

**Criador:** JuaMaker

---

## 📂 Estrutura de Arquivos

O projeto foi desenvolvido utilizando a extensão **PlatformIO** (VS Code) para melhor gerenciamento de bibliotecas e do sistema de arquivos na memória flash.

```text
4WD-JuaMaker/
├── data/                   # Arquivos do Frontend (Hospedados no ESP32 via LittleFS)
│   └── index.html          # Interface de controle UI (HTML, CSS e JS unificados)
├── include/
│   └── Motor.h             # Definições da classe Motor
├── src/
│   ├── main.cpp            # Core do sistema: WebServer, Roteamento, DNS e Integração
│   └── Motor.cpp           # Regra de negócio do Hardware (Driver PWM dos motores)
├── platformio.ini          # Configurações da placa, partições e dependências
└── README.md               # Documentação do projeto
```

---

## 🔌 Pinagem dos Motores

| Motor | Lado | IN1 | IN2 | Função |
|---|---|---:|---:|---|
| `motorFrenteEsq` | Esquerdo | GPIO 13 | GPIO 12 | Roda dianteira esquerda |
| `motorTrasEsq` | Esquerdo | GPIO 14 | GPIO 27 | Roda traseira esquerda |
| `motorFrenteDir` | Direito | GPIO 26 | GPIO 25 | Roda dianteira direita |
| `motorTrasDir` | Direito | GPIO 33 | GPIO 32 | Roda traseira direita |

> **Observação:** `IN1` e `IN2` correspondem às entradas de controle da ponte H associada a cada motor. A direção efetiva do motor depende da polaridade da ligação física e pode exigir a inversão dos sinais caso o sentido observado seja diferente do esperado.

---

## 📡 Conectividade e Servidor Web

A inteligência de rede do 4WD JuaMaker foi desenhada para funcionar "em campo", ou seja, em locais sem internet ou sem um roteador Wi-Fi por perto.

1. **Access Point (AP):** O ESP32 emite sua própria rede Wi-Fi chamada `JuaMaker_4WD`.
2. **Servidor DNS Interno (Captive DNS):** Em vez de obrigar o usuário a decorar o IP do robô (`192.168.4.1`), o ESP32 roda um servidor DNS na porta 53. Quando o celular ou computador conectado à rede do robô digita `http://juamaker.com` no navegador, o DNS intercepta e redireciona automaticamente para o servidor interno.
3. **LittleFS:** A página web (`index.html`) fica salva na memória Flash do ESP32 (LittleFS), separada do código C++. O servidor HTTP (`WebServer.h` na porta 80) apenas lê esse arquivo e o envia ao navegador do usuário.

---

## 💻 Interface de Controle (Frontend)

O painel de controle foi desenvolvido com design responsivo (adaptando-se a telas de celulares e PCs) com tema Dark Mode.

### Modos de Operação:
* **🕹️ Modo Livre:** Permite dirigir o robô em tempo real. Possui botões interativos na tela e suporte a atalhos de teclado (`W`, `A`, `S`, `D`). Ao segurar um botão, o comando é enviado; ao soltar, o comando de "Parar" é disparado instantaneamente.
* **⏱️ Modo Sequência (Estático):** Permite enviar comandos programados (ex: "Mover para Frente por 2 Unidades"). O ESP32 recebe a duração em milissegundos e controla o desligamento automático sem bloquear o servidor.

### Comunicação AJAX (`fetch`):
A página não recarrega para enviar comandos. Ao tocar em um botão de direção, o JavaScript utiliza a API `fetch` para chamar endpoints em segundo plano:
* **Endpoint:** `/api/move?cmd=[DIRECAO]&power=[1-100]&mode=[free|static]`
* **Controle de Sobrecarga:** Implementado um `AbortController` no JavaScript. Se o usuário metralhar os botões de controle, o navegador cancela as requisições antigas presas na fila e prioriza a última, garantindo que o ESP32 responda sem atraso (lag).

---

## ⚙️ Regra de Negócio dos Motores (Backend/Driver)

O robô possui 4 motores independentes, agrupados pelo código em Lado Esquerdo e Lado Direito.

### 1. Classe `Motor.cpp` (O Driver Base)
A classe abstrai a complexidade do hardware (Ponte H). 
* Recebe instâncias de dois pinos (`IN1` e `IN2`).
* A função `.setSpeed(int speed)` aceita valores de **-255 a 255**.
* Se o valor é positivo (>0): Aciona o PWM no IN1 (Frente).
* Se o valor é negativo (<0): Aciona o PWM no IN2 (Trás) de forma modular.
* Se é zero, corta a energia de ambos (Parada).

### 2. A Lógica de Movimento do 4WD (`main.cpp`)
Como um 4WD não possui um eixo de direção móvel (como carros comuns), as curvas são feitas pelo controle de velocidade relativo entre os lados (Skid Steer).

* **Avançar/Recuar:** Todos os 4 motores recebem a mesma potência.
* **Curvas Proporcionais:** Em vez de parar um lado ou invertê-lo bruscamente, o código calcula um `turnPwm` que equivale a **50% da potência atual**.
  * Ao curvar para a Esquerda (`L`): Os 2 motores da esquerda reduzem a potência pela metade, enquanto os 2 da direita continuam a 100%. Isso cria uma curva suave e fluida para o lado de menor tração.
  * Curvar para a Direita (`R`): Os 2 motores da esquerda mantêm 100%, os 2 da direita caem para 50%.

---

## 🚀 Como Compilar e Rodar

Para replicar ou gravar atualizações neste projeto:

1. Abra o projeto no **VS Code** com a extensão **PlatformIO** instalada.
2. Certifique-se de que os pinos dos motores instanciados no `main.cpp` correspondem à montagem física da sua placa.
3. No painel do PlatformIO (ícone da formiga), vá em: `Project Tasks > [seu_ambiente_esp32] > Platform`.
4. Clique em **`Build Filesystem Image`** e depois **`Upload Filesystem Image`**. Isso transferirá o arquivo `/data/index.html` para a memória interna.
5. Em seguida, na aba `General`, clique em **`Upload`** para compilar e enviar o firmware em C++.
6. Pelo seu celular/PC, conecte-se ao Wi-Fi do robô e acesse `http://juamaker.com`.


--- 

## 🗺️ Roadmap

O projeto ainda está em desenvolvimento. As próximas etapas estão organizadas
priorizando evolução da experiência de controle, robustez do sistema e
integração com novos componentes de hardware.

### 💻 Software e Interface Web

- [ ] **Controle via Gamepad API:** Permitir controlar o robô utilizando
      controles de videogame.

- [ ] **Controle de velocidade progressivo:** Permitir diferentes níveis
      de aceleração e desaceleração em vez de aplicar a potência diretamente.

- [ ] **Execução de sequências não bloqueante:** Substituir o uso de
      `delay()` por controle baseado em `millis()`, permitindo que o
      servidor continue processando requisições durante movimentos
      programados.

- [ ] **Gerenciamento de múltiplos clientes:** Definir o comportamento
      quando mais de um dispositivo estiver conectado simultaneamente,
      incluindo prioridade de controle e prevenção de comandos conflitantes.

- [ ] **OTA (Over-The-Air):** Permitir atualização do firmware pela rede
      sem necessidade de conexão USB.

### ⚙️ Hardware e Controle

- [ ] **Calibração dos motores:** Compensar diferenças de velocidade entre
      os motores para obter trajetórias mais consistentes.

- [ ] **Feedback visual e sonoro:** Adicionar LEDs, setas e/ou buzzer
      controláveis pela interface.

### 🤖 Sensoriamento e Autonomia

- [ ] **Sensor de distância:** Adicionar sensores ultrassônicos
      para detecção de obstáculos.

- [ ] **Frenagem automática:** Utilizar os sensores para reduzir ou
      interromper o movimento diante de obstáculos.