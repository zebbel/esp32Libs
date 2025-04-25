#include "button.h"

Button::Button(){
}

void Button::init(){
    gpio_set_direction((gpio_num_t)CONFIG_BUTTON_PIN, GPIO_MODE_INPUT);
}

void Button::loop(){
    if(!gpio_get_level((gpio_num_t)CONFIG_BUTTON_PIN) && state != BUTTON_LONG_PRESS){
        timeStamp = esp_timer_get_time();
        while(1){
            if(esp_timer_get_time() - timeStamp > 2000000){
                state = BUTTON_LONG_PRESS;
                break;
            } else if(gpio_get_level((gpio_num_t)CONFIG_BUTTON_PIN)){
                state = BUTTON_PRESSED;
                break;
            }
        }
    }else if(!gpio_get_level((gpio_num_t)CONFIG_BUTTON_PIN) && state == BUTTON_LONG_PRESS){
        while(!gpio_get_level((gpio_num_t)CONFIG_BUTTON_PIN));
        state = BUTTON_0;
    }else if(gpio_get_level((gpio_num_t)CONFIG_BUTTON_PIN)){
        state = BUTTON_0;
    }
}

buttonState_t Button::getState(){
    buttonState_t stateNow = state;
    if(state == BUTTON_PRESSED) state = BUTTON_0;
    return stateNow;
}

bool Button::pressed(){
    loop();
    if(state == BUTTON_PRESSED) return true;
    else return false;
}