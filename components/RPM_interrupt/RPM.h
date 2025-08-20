#ifndef RPM_H
#define RPM_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"

class RPM{
    public:
        float dt_s;
        float rpm;
        float mps;
        float kmh;
        RPM();
        void init(gpio_num_t sensorPin, uint16_t pulsesPerRev);
        void init(gpio_num_t sensorPin, uint16_t pulsesPerRev, float diameter);
        void update();
        
    private:
        gpio_num_t sensorGPIO;
        uint16_t pulsesPerRevolution;
        float wheel_circumference = 0;
        volatile uint64_t last_time;
        volatile uint64_t delta_time;

        static void IRAM_ATTR gpio_isr_handler(void* arg);
};

#endif