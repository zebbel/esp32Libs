#ifndef SERVO_H
#define SERVO_H

#include "driver/mcpwm_prelude.h"
#include "esp_mac.h"
#include <esp_log.h>

#define SERVO_TIMEBASE_50HZ 20000
#define SERVO_TIMEBASE_60HZ 16666
#define SERVO_TIMEBASE_100HZ 10000
#define SERVO_TIMEBASE_160HZ 6250
#define SERVO_TIMEBASE_333HZ 3003
#define SERVO_TIMEBASE_400HZ 2500

#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick

// Please consult the datasheet of your servo before changing the following parameters
#define SERVO_MIN_PULSEWIDTH_US 500  // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2500  // Maximum pulse width in microsecond
#define SERVO_MIN_DEGREE        -90   // Minimum angle
#define SERVO_MAX_DEGREE        90    // Maximum angle

class Servo{
    private:
        typedef struct{
            uint16_t minPulseWidth = 1000;
            uint16_t maxPulsWidth = 2000;
            int16_t minAngel = -45;
            int16_t maxAngel = 45;
        } servo_settings_t;

        mcpwm_timer_handle_t timer = NULL;
        mcpwm_timer_config_t timer_config;
        mcpwm_oper_handle_t oper = NULL;
        mcpwm_operator_config_t operator_config;
        mcpwm_cmpr_handle_t comparator = NULL;
        mcpwm_comparator_config_t comparator_config;
        mcpwm_gen_handle_t generator = NULL;
        mcpwm_generator_config_t generator_config;

        servo_settings_t servoSettings;

        uint32_t angleToUs(int32_t angle);

    public:
        Servo();
        void init(uint8_t GPIO);
        void init(uint8_t GPIO, uint16_t timeBase);
        void writeMicroseconds(uint32_t microseconds);
        void writeAngel(int32_t angel);
        void setLimits(uint16_t minPulseWidth, uint16_t maxPulsWidth, int16_t minAngel, int16_t maxAngel);
        void setFrequenzy(uint16_t timeBase);
};

#endif