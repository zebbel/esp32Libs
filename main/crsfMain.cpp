#include "esp_timer.h"

#include "crsf.h"

CRSF crsf;

void crsfMain(){
    crsf_channels_t channels;
    crsf_extended_data_t crsfExtendedFrame;
    crsf_altitude_t altitute = {1000, 0};

    crsf.init(UART_NUM_1);
    crsf_device_info_t deviceInfo;
    deviceInfo.deviceName = "ZSM";
    deviceInfo.serialNumber = 1;
    deviceInfo.hardwareId = 0;
    deviceInfo.firmwareId = 0;
    deviceInfo.parameterTotal = 1;
    deviceInfo.parameterVersion = 0;

    crsf.deviceInfo = &deviceInfo;

    uint64_t lastSend = esp_timer_get_time();

    while(1){
        crsf.receive_channels(&channels);
        //ESP_LOGI("main", "CH1: %d", crsf.channel_Mikroseconds(channels.ch1));

        if(xQueueReceive(crsf.extendedQueue, &crsfExtendedFrame, 0)){
            ESP_LOGI("main", "type: 0x%X, dest: 0x%X, src: 0x%X", crsfExtendedFrame.type, crsfExtendedFrame.dest, crsfExtendedFrame.src);
        }

        if((lastSend + 1000) < esp_timer_get_time()){
            lastSend = esp_timer_get_time();
            crsf.send_altitute(&altitute);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}