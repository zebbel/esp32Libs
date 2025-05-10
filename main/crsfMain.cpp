#include "esp_timer.h"

#include "crsf.h"

CRSF crsf;

uint8_t testData = 5;

crsf_command_status_t testCallback(){
    ESP_LOGI("testCallback", "juup, %i", testData);
    return CRSF_COMMAND_READY;
}

void crsfMain(){
    crsf_channels_t channels;
    crsf_altitude_t altitute = {1000, 0};

    crsf.init(UART_NUM_1, "ZSM");

    crsf_parameter_folder_t folder = {
        .name = "Folder"
    };
    crsf.register_parameter(&folder);

    crsf_parameter_folder_t subFolder = {
        .name = "Sub Folder"
    };
    crsf.register_parameter(&subFolder, folder);

    crsf_parameter_uint8_t valParameter = {
        .name = "Test",
        .value = &testData,
        .min = 0,
        .max = 10,
        .unit = "%"
    };
    crsf.register_parameter(&valParameter, subFolder);

    
    crsf_parameter_command_t testParamter = {
        .name = "Wifi",
        .status = CRSF_COMMAND_READY,
        .timeout = 100,
        .info = "",
        .callback = testCallback
    };
    crsf.register_parameter(&testParamter, folder);
    
    crsf_parameter_uint8_t valParameter2 = {
        .name = "Test",
        .value = &testData,
        .min = 0,
        .max = 10,
        .unit = "%"
    };
    crsf.register_parameter(&valParameter2);

    uint64_t lastSend = esp_timer_get_time();

    while(1){
        crsf.receive_channels(&channels);
        //ESP_LOGI("main", "CH1: %d", crsf.channel_Mikroseconds(channels.ch1));

        if((lastSend + 1000) < esp_timer_get_time()){
            lastSend = esp_timer_get_time();
            crsf.send_altitute(&altitute);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}