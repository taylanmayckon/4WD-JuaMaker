#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <DNSServer.h>
#include "Motor.h"

// Credenciais da rede Wi-Fi 
const char* ap_ssid = "JuaMaker_4WD";
const char* ap_pass = "12345678"; 
// Nome para acessar a ESP32 via mDNS 
const char* host_name = "juamaker.com";

const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer server(80);

void handleRoot() {
  server.send(200, "text/html; charset=utf-8", 
              "<h1>Bem-vindo ao painel da ESP32!</h1>"
              "<p>Acessado com sucesso via nome de domínio amigável.</p>");
}

// Instanciando os 4 motores com seus respectivos pinos
Motor motorFrenteEsq(13, 12);
Motor motorTrasEsq(14, 27);
Motor motorFrenteDir(26, 25);
Motor motorTrasDir(33, 32);

void setup() {
    Serial.begin(115200);

    // Inicia o Wi-Fi da ESP32
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_pass);

    IPAddress apIP = WiFi.softAPIP();
    Serial.print("Rede ativa. IP da placa: ");
    Serial.println(apIP);

    // Inicia o DNS Server 
    // Usar "*" no primeiro parâmetro redireciona QUALQUER domínio digitado para a ESP32 (estilo Captive Portal)
    dnsServer.start(DNS_PORT, host_name, apIP);

    server.on("/", HTTP_GET, handleRoot);

    // Redireciona 404 para a página principal se digitar algo fora do padrão
    server.onNotFound([]() {
      server.send(200, "text/html; charset=utf-8", "<h1>Página inicial</h1><a href='/'>Ir para o início</a>");
    });

    server.begin();
    Serial.printf("Acesse pelo navegador em: http://%s\n", host_name);



    // Motores 4WD
    Serial.println("Inicializando Motores 4WD...");
    // Inicializa o hardware de cada motor
    motorFrenteEsq.begin();
    motorTrasEsq.begin();
    motorFrenteDir.begin();
    motorTrasDir.begin();
}

void loop() {
    // Mantém o DNS Server e o WebServer ativos para responder às requisições
    dnsServer.processNextRequest();
    server.handleClient();

    // Serial.println("Acelerando para frente...");
    // motorFrenteEsq.setSpeed(255);
    // motorTrasEsq.setSpeed(255);
    // motorFrenteDir.setSpeed(255);
    // motorTrasDir.setSpeed(255);
    
    // delay(3000);

    // Serial.println("Parando...");
    // motorFrenteEsq.stop();
    // motorTrasEsq.stop();
    // motorFrenteDir.stop();
    // motorTrasDir.stop();
    
    // delay(2000);
    
    // Serial.println("Dando ré...");
    // motorFrenteEsq.setSpeed(-255);
    // motorTrasEsq.setSpeed(-255);
    // motorFrenteDir.setSpeed(-255);
    // motorTrasDir.setSpeed(-255);
    
    // delay(3000);
    
    // motorFrenteEsq.stop();
    // motorTrasEsq.stop();
    // motorFrenteDir.stop();
    // motorTrasDir.stop();
    
    // delay(2000);
}