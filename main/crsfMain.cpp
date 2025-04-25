#include "CRSF.h"

CRSF crsf;

void crsfMain(){
    QueueHandle_t crsfQueue = xQueueCreate(5, sizeof(crsf_frame_t));
    crsf_channels_t channels;
    crsf_frame_t crsfFrame;

    crsf.init(UART_NUM_1, &crsfQueue);

    while(1){
        crsf.receive_channels(&channels);
        ESP_LOGI("main", "CH1: %d", crsf.channel_Mikroseconds(channels.ch1));

        if(xQueueReceive(crsfQueue, &crsfFrame, 0)){
            ESP_LOGI("main", "PARAMETER_WRITE: destAddr: 0x%X, parameter: 0x%X, value: 0x%X", crsfFrame.destAddr, crsfFrame.payload[0], crsfFrame.payload[1]);
        }


        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}