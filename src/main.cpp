#include <Arduino.h>
#include "Motor.h"

// Instanciando os 4 motores com seus respectivos pinos
Motor motorFrenteEsq(13, 12);
Motor motorTrasEsq(14, 27);
Motor motorFrenteDir(26, 25);
Motor motorTrasDir(33, 32);

#define IN1 13
#define IN2 12

void setup() {
    Serial.begin(115200);
    Serial.println("Inicializando Motores 4WD...");

    // Inicializa o hardware de cada motor
    motorFrenteEsq.begin();
    motorTrasEsq.begin();
    motorFrenteDir.begin();
    motorTrasDir.begin();
}

void loop() {
    Serial.println("Acelerando para frente...");
    motorFrenteEsq.setSpeed(255);
    motorTrasEsq.setSpeed(255);
    motorFrenteDir.setSpeed(255);
    motorTrasDir.setSpeed(255);
    
    delay(3000);

    Serial.println("Parando...");
    motorFrenteEsq.stop();
    motorTrasEsq.stop();
    motorFrenteDir.stop();
    motorTrasDir.stop();
    
    delay(2000);
    
    Serial.println("Dando ré...");
    motorFrenteEsq.setSpeed(-255);
    motorTrasEsq.setSpeed(-255);
    motorFrenteDir.setSpeed(-255);
    motorTrasDir.setSpeed(-255);
    
    delay(3000);
    
    motorFrenteEsq.stop();
    motorTrasEsq.stop();
    motorFrenteDir.stop();
    motorTrasDir.stop();
    
    delay(2000);
}