#include "RPM2.h"

// Interrupt-Handler
void IRAM_ATTR RPM2::gpio_isr_handler(void* arg){
    RPM2* rpmClass = reinterpret_cast<RPM2*>(arg); //obtain the instance pointer
    ESP_ERROR_CHECK(pcnt_unit_get_count(rpmClass->pcnt_unit, &rpmClass->counter));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(rpmClass->pcnt_unit));

    rpmClass->newValue = true;
}

RPM2::RPM2(){
    
}

void RPM2::init(gpio_num_t sensorPin, gpio_num_t rmtPin, uint16_t pulsesPerRev){
    sensorGPIO = sensorPin;
    rmtGPIO = rmtPin;
    pulsesPerRevolution = pulsesPerRev;

    configPCNT();
    configRMT();
    configIRQ();
}

uint16_t RPM2::getCount(){
    newValue = false;
    return counter;
}

float RPM2::getRpm(){
    newValue = false;
    float revs = static_cast<float>(counter) / pulsesPerRevolution;
    revs = revs * (60000.0f / 100);
    return revs;
}

void RPM2::configPCNT(){
    pcnt_unit_config_t unit_config = {
        .low_limit = -2000,
        .high_limit = 2000,
        .intr_priority = 0,
        .flags = {}
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = sensorGPIO,
        .level_gpio_num = rmtGPIO,
        .flags = {}
    };
    pcnt_channel_handle_t pcnt_chan = nullptr;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(
        pcnt_chan,
        PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        PCNT_CHANNEL_EDGE_ACTION_INCREASE
    ));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
}

void RPM2::configRMT(){
    // 1) RMT TX-Kanal konfigurieren
    rmt_tx_channel_config_t tx_chan_cfg = {
        .gpio_num = rmtGPIO,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 500000, // 2 µs
        .mem_block_symbols = 64,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .flags = { .invert_out = 0, .with_dma = 0, .io_loop_back = 1, .io_od_mode = 0, .allow_pd = 0 }
    };
    tx_chan = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_cfg, &tx_chan));

    // 2) Copy-Encoder erstellen
    rmt_copy_encoder_config_t encoder_cfg = {};
    encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&encoder_cfg, &encoder));

    // 3) Kanal aktivieren
    ESP_ERROR_CHECK(rmt_enable(tx_chan));

    // 4) Pattern erstellen (HIGH in 15-Bit Segmenten + LOW)
    raw_symbols[0] = {.duration0 = 30000, .level0 = 1, .duration1 = 19990, .level1 = 1};
    raw_symbols[1] = {.duration0 = 10, .level0 = 0, .duration1 = 0, .level1 = 0};

    // 5) Endlos-Wiederholung konfigurieren
    tx_config = {
        .loop_count = -1, // -1 = unendlich
        .flags = { .eot_level = 0, .queue_nonblocking = 0 }
    };
}

void RPM2::start_10Hz(){
    ESP_ERROR_CHECK(rmt_disable(tx_chan));
    ESP_ERROR_CHECK(rmt_enable(tx_chan));

    raw_symbols->duration0 = 30000;
    raw_symbols->level0 = 1;
    raw_symbols->duration1 = 19990;
    raw_symbols->level1 = 1;
    ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, raw_symbols, sizeof(raw_symbols), &tx_config));
}

void RPM2::start_20Hz(){
    ESP_ERROR_CHECK(rmt_disable(tx_chan));
    ESP_ERROR_CHECK(rmt_enable(tx_chan));

    raw_symbols->duration0 = 5000;
    raw_symbols->level0 = 1;
    raw_symbols->duration1 = 19990;
    raw_symbols->level1 = 1;
    ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, raw_symbols, sizeof(raw_symbols), &tx_config));
}

void RPM2::start_40Hz(){
    ESP_ERROR_CHECK(rmt_disable(tx_chan));
    ESP_ERROR_CHECK(rmt_enable(tx_chan));

    raw_symbols->duration0 = 12490;
    raw_symbols->level0 = 1;
    raw_symbols->duration1 = 0;
    raw_symbols->level1 = 0;
    ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, raw_symbols, sizeof(raw_symbols), &tx_config));
}

void RPM2::start_50Hz(){
    ESP_ERROR_CHECK(rmt_disable(tx_chan));
    ESP_ERROR_CHECK(rmt_enable(tx_chan));

    raw_symbols->duration0 = 9990;
    raw_symbols->level0 = 1;
    raw_symbols->duration1 = 0;
    raw_symbols->level1 = 0;
    ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, raw_symbols, sizeof(raw_symbols), &tx_config));
}

void RPM2::start_75Hz(){
    ESP_ERROR_CHECK(rmt_disable(tx_chan));
    ESP_ERROR_CHECK(rmt_enable(tx_chan));

    raw_symbols[0] = {.duration0 = 6640, .level0 = 1, .duration1 = 0, .level1 = 0};
    ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, raw_symbols, sizeof(raw_symbols), &tx_config));
}

void RPM2::start_100Hz(){
    ESP_ERROR_CHECK(rmt_disable(tx_chan));
    ESP_ERROR_CHECK(rmt_enable(tx_chan));

    raw_symbols->duration0 = 4990;
    raw_symbols->level0 = 1;
    raw_symbols->duration1 = 0;
    raw_symbols->level1 = 0;
    ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, raw_symbols, sizeof(raw_symbols), &tx_config));
}

void RPM2::configIRQ(){
    // 1. aktiviere interrupt an gpio
    gpio_set_intr_type(rmtGPIO, GPIO_INTR_NEGEDGE);

    // 2. ISR-Handler installieren
    gpio_install_isr_service(0);

    // 3. ISR-Handler für GPIO4 registrieren
    gpio_isr_handler_add(rmtGPIO, &RPM2::gpio_isr_handler, this);
}