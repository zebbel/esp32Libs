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

        RPM2();
        void init(gpio_num_t sensorPin, gpio_num_t rmtPin, uint16_t pulsesPerRev);
        void start_10Hz();
        void start_20Hz();
        void start_40Hz();
        void start_50Hz();
        void start_75Hz();
        void start_100Hz();
        float getRpm();
        uint16_t getCount();
    private:
        rmt_symbol_word_t raw_symbols[2];
        
        int counter = 0;
        float rpm = 0.0;

        pcnt_unit_handle_t pcnt_unit = nullptr;
        rmt_transmit_config_t tx_config;
        rmt_channel_handle_t tx_chan;
        rmt_encoder_handle_t encoder;

        gpio_num_t sensorGPIO;
        gpio_num_t rmtGPIO;
        uint16_t pulsesPerRevolution;

        static void IRAM_ATTR gpio_isr_handler(void* arg);
        void configPCNT();
        void configRMT();
        void configIRQ();
};

#endif