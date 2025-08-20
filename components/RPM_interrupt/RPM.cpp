#include "RPM.h"

// Interrupt-Handler
void IRAM_ATTR RPM::gpio_isr_handler(void* arg){
    RPM* rpmClass = reinterpret_cast<RPM*>(arg); //obtain the instance pointer
    uint64_t now = esp_timer_get_time(); // Zeit in Mikrosekunden
    if(now - rpmClass->last_time > 300){
        rpmClass->delta_time = now - rpmClass->last_time;
        rpmClass->last_time = now;
    }
}

RPM::RPM(){

}

void RPM::init(gpio_num_t sensorPin, uint16_t pulsesPerRev){
    sensorGPIO = sensorPin;
    pulsesPerRevolution = pulsesPerRev;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE; // steigende Flanke
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << sensorGPIO);
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    gpio_install_isr_service(0); // default ISR service
    gpio_isr_handler_add(sensorGPIO, &RPM::gpio_isr_handler, this);
}

void RPM::init(gpio_num_t sensorPin, uint16_t pulsesPerRev, float diameter){
    wheel_circumference = 3.1416f * diameter;
    init(sensorPin, pulsesPerRev);
}

void RPM::update(){
    uint64_t dt_us = delta_time;
    if (dt_us > 0) {
        dt_s = dt_us / 1000000.0f;
        rpm = 60.0 / (pulsesPerRevolution * dt_s);
        m_s = wheel_circumference * (rpm / 60.0);
    }
}