#include <stdio.h>
#include "Servo.h"
#include <freertos/FreeRTOS.h>

Servo servo;

void servoMain(){
    servo.init(0);
    
    uint16_t value = 1500;
    int8_t step = 10;

    while(1){
        vTaskDelay(500 / portTICK_PERIOD_MS);

        if ((value + step) >= 2000 || (value + step) <= 1000) {
            step *= -1;
        }
        value += step;

        servo.writeMicroseconds(value);
    }
}