#include "Motor.h"

Motor::Motor(uint8_t in1, uint8_t in2) {
    pinIN1 = in1;
    pinIN2 = in2;
}

void Motor::begin() {
    pinMode(pinIN1, OUTPUT);
    pinMode(pinIN2, OUTPUT);
    stop(); // Garante que inicie desligado
}

void Motor::setSpeed(int speed) {
    if (speed > 255) speed = 255;
    if (speed < -255) speed = -255;

    if (speed > 0) {
        // Frente
        analogWrite(pinIN1, speed);
        analogWrite(pinIN2, 0); // Funciona igual ao digitalWrite LOW
    } else if (speed < 0) {
        // Trás
        analogWrite(pinIN1, 0); // Funciona igual ao digitalWrite LOW
        analogWrite(pinIN2, -speed);
    } else {
        // Parado
        analogWrite(pinIN1, 0);
        analogWrite(pinIN2, 0);
    }
}

void Motor::stop() {
    analogWrite(pinIN1, 0);
    analogWrite(pinIN2, 0);
}