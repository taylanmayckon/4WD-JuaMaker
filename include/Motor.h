#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor {
private:
    uint8_t pinIN1;
    uint8_t pinIN2;

public:
    // Construtor recebe os dois pinos da ponte H
    Motor(uint8_t in1, uint8_t in2);
    
    // Configura os pinos como saída
    void begin();
    
    // Velocidade: > 0 (Frente), < 0 (Trás), 0 (Parado)
    void setSpeed(int speed);
    
    // Para o motor imediatamente
    void stop();
};

#endif