#ifndef BUTTON_H
#define BUTTON_H

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

//static const char *buttonTag = "button";

typedef enum {
    BUTTON_0,
    BUTTON_PRESSED,
    BUTTON_LONG_PRESS
} buttonState_t;

class Button{
    private:
        uint64_t timeStamp = 0;

    public:
        buttonState_t state = BUTTON_0;

        Button();
        void init();
        void loop();
        buttonState_t getState();
        bool pressed();

};

#endif