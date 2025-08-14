#include "esp_timer.h"

#include "Hobbywing.h"
#include "crsf.h"

CRSF crsf;
crsf_channels_t channels;
Hobbywing hobbywing;

crsf_battery_t battery;

void hobbywingMain(){
    crsf.init(UART_NUM_1, "ZSM", &channels);
    hobbywing.init(UART_NUM_2);

    uint64_t lastSend = esp_timer_get_time();

    while(1){
        //srxl2.channelValue = crsf.channel_Mikroseconds(channels.ch2);

        if((lastSend + 1000) < esp_timer_get_time()){
            lastSend = esp_timer_get_time();
            //battery.voltage = srxl2.sensors.voltsInput;
            crsf.send_battery(&battery);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}