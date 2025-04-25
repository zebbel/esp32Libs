#include "Servo.h"

static const char *TAG = "servo";

/**
 * @brief servo constructor
 */
Servo::Servo(){
}

/**
 * @brief initialize servo
 * 
 * @param GPIO: gpio number
 */
void Servo::init(uint8_t GPIO){
    init(GPIO, SERVO_TIMEBASE_50HZ);
}


/**
 * @brief initialize servo
 * 
 * @param GPIO: gpio number
 * @param timeBase: servo refresh rate
 */
void Servo::init(uint8_t GPIO, uint16_t timeBase){
    ESP_LOGI(TAG, "Create timer and operator");
    timer_config.group_id = 0;
    timer_config.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
    timer_config.resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ;
    timer_config.count_mode = MCPWM_TIMER_COUNT_MODE_UP;
    timer_config.period_ticks = timeBase;

    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    operator_config.group_id = 0; // operator must be in the same group to the timer

    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &oper));

    ESP_LOGI(TAG, "Connect timer and operator");
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));
    
    ESP_LOGI(TAG, "Create comparator and generator from the operator");
    comparator_config.flags.update_cmp_on_tez = true;
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparator));

    generator_config.gen_gpio_num = GPIO;
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &generator_config, &generator));

    // set the initial compare value, so that the servo will spin to the center position
    //ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, example_angle_to_compare(0)));

    ESP_LOGI(TAG, "Set generator action on timer and compare event");
    // go high on counter empty
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generator, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    // go low on compare threshold
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generator, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW)));

    ESP_LOGI(TAG, "Enable and start timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
}

/**
 * @brief convert angel to microseconds for servo control
 * 
 * @param angle: given angel
 * @returns microseconds
 */
uint32_t Servo::angleToUs(int32_t angle){
    return (angle - servoSettings.minAngel) * (servoSettings.maxPulsWidth - servoSettings.minPulseWidth) / (servoSettings.maxAngel - servoSettings.minAngel) + servoSettings.minPulseWidth;
}

/**
 * @brief set servo position
 * 
 * @param ms: microseconds
 */
void Servo::writeMicroseconds(uint32_t microseconds){
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, microseconds));
}

/**
 * @brief set servo position
 * 
 * @param angel
 */
void Servo::writeAngel(int32_t angel){
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, angleToUs(angel)));
}

/**
 * @brief set limits when writing servo with writeAngel
 * 
 * @param minPulseWidth
 * @param maxPulsWidth
 * @param minAngel
 * @param maxAngel
 */
void Servo::setLimits(uint16_t minPulseWidth, uint16_t maxPulsWidth, int16_t minAngel, int16_t maxAngel){
    servoSettings.minPulseWidth = minPulseWidth;
    servoSettings.maxPulsWidth = maxPulsWidth;
    servoSettings.minAngel = minAngel;
    servoSettings.maxAngel = maxAngel;
}

/**
 * @brief set servo frequenzy
 * 
 * @param timeBase: SERVO_TIMEBASE_50HZ, SERVO_TIMEBASE_60HZ, SERVO_TIMEBASE_100HZ, SERVO_TIMEBASE_160HZ, SERVO_TIMEBASE_333HZ, SERVO_TIMEBASE_400HZ
 */
void Servo::setFrequenzy(uint16_t timeBase){
    timer_config.period_ticks = timeBase;
}