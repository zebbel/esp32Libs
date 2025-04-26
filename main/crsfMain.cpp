#include "CRSF.h"

CRSF crsf;

void crsfMain(){
    crsf_channels_t channels;
    crsf_extended_t crsfFrame;

    crsf.init(UART_NUM_1);

    while(1){
        //crsf.receive_channels(&channels);
        //ESP_LOGI("main", "CH1: %d", crsf.channel_Mikroseconds(channels.ch1));

        if(xQueueReceive(crsf.extendedQueue, &crsfFrame, 0)){
            ESP_LOGI("main", "type: 0x%X, dest: 0x%X, src: 0x%X", crsfFrame.type, crsfFrame.dest, crsfFrame.src);
        }


        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}