#ifndef RPM2_H
#define RPM2_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"

class RPM2{
    public:
        float dt_s;
        float rpm;
        RPM2();
        void init(gpio_num_t sensorPin, uint16_t pulsesPerRev);
        void update();
        
    private:
        gpio_num_t sensorGPIO;
        uint16_t pulsesPerRevolution;
        volatile uint64_t last_time;
        volatile uint64_t delta_time;

        static void IRAM_ATTR gpio_isr_handler(void* arg);
};

#endif