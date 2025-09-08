#include "RPM.h"

// Interrupt-Handler
void IRAM_ATTR RPM::gpio_isr_handler(void* arg){
    RPM* rpmClass = reinterpret_cast<RPM*>(arg); //obtain the instance pointer
    uint64_t now = esp_timer_get_time(); // Zeit in Mikrosekunden

    if(now - rpmClass->last_time > 300 && now - rpmClass->last_time < 20000){
        //rpmClass->delta_time = now - rpmClass->last_time;
        rpmClass->sum -= rpmClass->ringbuffer[rpmClass->head];
        rpmClass->ringbuffer[rpmClass->head] = now - rpmClass->last_time;
        rpmClass->head = (rpmClass->head + 1) % RPM_AVERAGE_SIZE;
        rpmClass->sum += now - rpmClass->last_time;
    }

    rpmClass->last_time = now;
}

RPM::RPM(){

}

void RPM::init(gpio_num_t sensorPin, uint16_t pulsesPerRev){
    sensorGPIO = sensorPin;
    pulsesPerRevolution = pulsesPerRev;

    for(uint8_t i=0; i<RPM_AVERAGE_SIZE; i++){
        ringbuffer[i] = 0;
    }

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
    wheel_circumference = 3.1415926f * diameter;
    init(sensorPin, pulsesPerRev);
}

void RPM::update(){
    /*
    uint64_t dt_us = delta_time;
    if (dt_us > 0) {
        dt_s = dt_us / 1000000.0f;
        rpm = 60.0 / (pulsesPerRevolution * dt_s);
        mps = wheel_circumference * (rpm / 60.0);
        kmh = mps * 3.6;
    }
    */
    
    //const uint64_t dt_us = delta_time;

    uint64_t dt_us = sum / RPM_AVERAGE_SIZE;
    /*
    for(size_t i=0; i<RPM_AVERAGE_SIZE; i++){
        dt_us += ringbuffer[i];
    }
    dt_us /= RPM_AVERAGE_SIZE;
    */

    // Check if timeout passed since last pulse
    if(esp_timer_get_time() - last_time > adaptive_timeout) {
        dts = 0.0f;
        rpm = 0.0f;
        mps = 0.0f;
        kmh = 0.0f;
        for(uint8_t i=0; i<RPM_AVERAGE_SIZE; i++){
        ringbuffer[i] = 0;
    }
        return;
    }

    if (dt_us > 0) {
        // precomputed reciprocals
        static constexpr float INV_US  = 1.0f / 1000000.0f;  // 1e-6
        static constexpr float INV_60  = 1.0f / 60.0f;
        static constexpr float KMH_PER_MPS = 3.6f;

        dts = float(dt_us) * INV_US;                        // multiply instead of divide
        const float rev_per_sec = INV_60 * (60.0f / pulsesPerRevolution) / dts; // constant folding happens
        rpm = rev_per_sec / INV_60;                          // == rev_per_sec * 60
        mps = wheel_circumference * (rpm * INV_60);          // rpm/60 via multiply
        kmh = mps * KMH_PER_MPS;

        adaptive_timeout = dt_us * 6;
    }
}