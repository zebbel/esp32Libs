#include "esp_timer.h"

#include "crsf.h"

CRSF crsf;


crsf_command_status_t testCallback(){
    ESP_LOGI("testCallback", "juup");
    return CRSF_COMMAND_READY;
}

void crsfMain(){
    crsf_channels_t channels;
    crsf_altitude_t altitute = {1000, 0};

    crsf.init(UART_NUM_1, "ZSM");

    crsf_parameter_uint8_t valParameter = {
        .common = {1, 0, 0, CRSF_UINT8, "Test"},
        .value = 5,
        .min = 0,
        .max = 10,
        .unit = "%"
    };
    crsf.registerParameter(valParameter.common.dataType, (int*)&valParameter);

    
    crsf_parameter_command_t testParamter = {
        .common = {2, 0, 0, CRSF_COMMAND, "Wifi"},
        .status = CRSF_COMMAND_READY,
        .timeout = 100,
        .info = "",
        .callback = testCallback
    };
    crsf.registerParameter(testParamter.common.dataType, (int*)&testParamter);
    
    

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