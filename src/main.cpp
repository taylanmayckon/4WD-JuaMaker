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

// -> MOTORES
// Instanciando os 4 motores com seus respectivos pinos
Motor motorFrenteEsq(13, 12);
Motor motorTrasEsq(14, 27);
Motor motorFrenteDir(26, 25);
Motor motorTrasDir(33, 32);


// -> CONTROLE WEB
// Potência atual em porcentagem
int motorPower = 100;

// Converte porcentagem (0-100) para PWM (0-255)
int powerToPWM(int percent) {
    percent = constrain(percent, 0, 100);
    return map(percent, 0, 100, 0, 255);
}

// -> Para todos os motores
void stopMotors() {
    motorFrenteEsq.stop();
    motorTrasEsq.stop();
    motorFrenteDir.stop();
    motorTrasDir.stop();
}

// -> Movimento para frente
void moveForward(int power) {

    motorFrenteEsq.setSpeed(power);
    motorTrasEsq.setSpeed(power);

    motorFrenteDir.setSpeed(power);
    motorTrasDir.setSpeed(power);
}

// -> Movimento para trás
void moveBackward(int power) {

    motorFrenteEsq.setSpeed(-power);
    motorTrasEsq.setSpeed(-power);

    motorFrenteDir.setSpeed(-power);
    motorTrasDir.setSpeed(-power);
}

// -> Curva para esquerda
void turnLeft(int power) {

    int innerPower = power / 2;

    motorFrenteEsq.setSpeed(innerPower);
    motorTrasEsq.setSpeed(innerPower);

    motorFrenteDir.setSpeed(power);
    motorTrasDir.setSpeed(power);
}

// -> Curva para direita
void turnRight(int power) {

    int innerPower = power / 2;

    motorFrenteEsq.setSpeed(power);
    motorTrasEsq.setSpeed(power);

    motorFrenteDir.setSpeed(innerPower);
    motorTrasDir.setSpeed(innerPower);
}


// -> API
void handleMove() {

    if (!server.hasArg("cmd")) {
        server.send(400, "text/plain", "Parametro cmd ausente");
        return;
    }

    String cmd = server.arg("cmd");

    // --------------------------------------------------------
    // Potência enviada pelo site
    // --------------------------------------------------------

    if (server.hasArg("power")) {
        motorPower = constrain(server.arg("power").toInt(), 0, 100);
    }

    int pwm = powerToPWM(motorPower);


    // --------------------------------------------------------
    // Comando de potência
    // --------------------------------------------------------

    if (cmd == "P") {

        Serial.print("Potencia: ");
        Serial.print(motorPower);
        Serial.print("% -> PWM ");
        Serial.println(pwm);

        server.send(200, "text/plain", "Potencia atualizada");
        return;
    }


    // --------------------------------------------------------
    // STOP
    // --------------------------------------------------------

    if (cmd == "S") {

        stopMotors();

        Serial.println("STOP");

        server.send(200, "text/plain", "Parado");
        return;
    }


    // --------------------------------------------------------
    // MOVIMENTOS
    // --------------------------------------------------------

    if (cmd == "F") {

        moveForward(pwm);

        Serial.print("FRENTE - ");
        Serial.println(motorPower);

    } 
    else if (cmd == "B") {

        moveBackward(pwm);

        Serial.print("TRAS - ");
        Serial.println(motorPower);

    } 
    else if (cmd == "L") {

        turnLeft(pwm);

        Serial.print("ESQUERDA - ");
        Serial.println(motorPower);

    } 
    else if (cmd == "R") {

        turnRight(pwm);

        Serial.print("DIREITA - ");
        Serial.println(motorPower);

    } 
    else {

        server.send(400, "text/plain", "Comando invalido");
        return;
    }

    // --------------------------------------------------------
    // Movimento estático
    // --------------------------------------------------------
    //
    // Para o protótipo podemos deixar o servidor aguardando
    // durante a duração solicitada.
    //
    // Exemplo:
    // /api/move?cmd=F&power=80&mode=static&duration=1000
    //
    // Depois de 1000 ms os motores param.
    // --------------------------------------------------------

    if (server.hasArg("mode") && server.arg("mode") == "static") {

        if (server.hasArg("duration")) {

            int duration = server.arg("duration").toInt();

            if (duration < 0) {
                duration = 0;
            }

            Serial.print("Movimento estatico por ");
            Serial.print(duration);
            Serial.println(" ms");

            delay(duration);

            stopMotors();

            Serial.println("Movimento estatico finalizado");
        }
    }

    server.send(200, "text/plain", "OK");
}


// -> Serve o site
void handleRoot() {
    if (!LittleFS.exists("/index.html")) {
        server.send(500, "text/plain", "index.html NAO existe no LittleFS");
        return;
    }

    File file = LittleFS.open("/index.html", "r");

    if (!file) {
        server.send(500, "text/plain", "Erro ao abrir index.html");
        return;
    }

    Serial.print("Enviando index.html - tamanho: ");
    Serial.println(file.size());

    server.streamFile(file, "text/html; charset=utf-8");

    file.close();
}


// -> Setup
void setup() {
    Serial.begin(115200);

    // LittleFS
     if (!LittleFS.begin(true)) {
        Serial.println("Erro ao iniciar LittleFS");
        return;
    }
    Serial.println("LittleFS iniciado");

    // Wi-Fi
    WiFi.mode(WIFI_AP);

    WiFi.softAP(ap_ssid, ap_pass);

    IPAddress apIP = WiFi.softAPIP();

    Serial.print("Rede ativa. IP da placa: ");
    Serial.println(apIP);

    // DNS Server
    dnsServer.start(
        DNS_PORT,
        host_name,
        apIP
    );

    // WebServer
    server.on(
        "/",
        HTTP_GET,
        handleRoot
    );

    // API de controle dos motores
    server.on(
        "/api/move",
        HTTP_GET,
        handleMove
    );


    // Arquivos estáticos
    server.serveStatic(
        "/",
        LittleFS,
        "/"
    );

    // 404
    server.onNotFound([]() {

        // Se o navegador pedir algo que não existe,
        // volta para a página principal.

        if (LittleFS.exists("/index.html")) {

            File file = LittleFS.open("/index.html", "r");

            if (file) {
                server.streamFile(file, "text/html; charset=utf-8");
                file.close();
            } else {
                server.send(
                    404,
                    "text/plain",
                    "Pagina nao encontrada"
                );
            }

        } 
        else {

            server.send(
                404,
                "text/plain",
                "Pagina nao encontrada"
            );
        }
    });

    server.begin();

    Serial.printf(
        "Acesse pelo navegador em: http://%s\n",
        host_name
    );

    // Motores 4WD
    Serial.println("Inicializando Motores 4WD...");
    // Inicializa o hardware de cada motor
    motorFrenteEsq.begin();
    motorTrasEsq.begin();
    motorFrenteDir.begin();
    motorTrasDir.begin();
    stopMotors();
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}