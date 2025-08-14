#ifndef RPM_H
#define RPM_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "driver/rmt_tx.h"

class RPM{
    public:
        bool newValue = false;

        RPM();
        void init(gpio_num_t sensorPin, gpio_num_t rmtPin, uint16_t pulsesPerRev);
        float getRpm();
        uint16_t getCount();
    private:
        int counter = 0;
        float rpm = 0.0;

        pcnt_unit_handle_t pcnt_unit = nullptr;

        gpio_num_t sensorGPIO;
        gpio_num_t rmtGPIO;
        uint16_t pulsesPerRevolution;

        static void IRAM_ATTR gpio_isr_handler(void* arg);
        void configPCNT();
        void configRMT();
        void configIRQ();
};

#endif