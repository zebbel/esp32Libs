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
        void start_100ms();
        void start_50ms();
        void start_25ms();
        void start_20ms();
        void start_10ms();
        float getRpm();
        uint16_t getCount();
    private:
        //100Hz
        rmt_symbol_word_t raw_symbols_10ms[2] = {{.duration0 = 4990, .level0 = 1, .duration1 = 0, .level1 = 1}, {.duration0 = 10, .level0 = 0, .duration1 = 0, .level1 = 0},};
        //50Hz
        rmt_symbol_word_t raw_symbols_20ms[2] = {{.duration0 = 9990, .level0 = 1, .duration1 = 0, .level1 = 1}, {.duration0 = 10, .level0 = 0, .duration1 = 0, .level1 = 0},};
        //40Hz
        rmt_symbol_word_t raw_symbols_25ms[2] = {{.duration0 = 12490, .level0 = 1, .duration1 = 0, .level1 = 1}, {.duration0 = 10, .level0 = 0, .duration1 = 0, .level1 = 0},};
        //20Hz
        rmt_symbol_word_t raw_symbols_50ms[2] = {{.duration0 = 5000, .level0 = 1, .duration1 = 19990, .level1 = 1}, {.duration0 = 10, .level0 = 0, .duration1 = 0, .level1 = 0},};
        //10Hz
        rmt_symbol_word_t raw_symbols_100ms[2] = {{.duration0 = 30000, .level0 = 1, .duration1 = 19990, .level1 = 1}, {.duration0 = 10, .level0 = 0, .duration1 = 0, .level1 = 0},};

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