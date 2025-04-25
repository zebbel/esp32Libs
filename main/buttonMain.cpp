#include <freertos/FreeRTOS.h>
#include "button.h"

Button button;

void buttonMain(){
    button.init();

    while(1){

        button.loop();
        switch (button.getState()){
            case BUTTON_PRESSED:
                ESP_LOGI("main", "button pressed");
                break;
            case BUTTON_LONG_PRESS:
                ESP_LOGI("main", "button long pressed");
                break;
            default:
                break;
        }

/*
        bool status = button.pressed();
        ESP_LOGI(mainTag, "%d", status);
*/

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}