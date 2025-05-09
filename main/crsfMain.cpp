#include "esp_timer.h"

#include "crsf.h"

CRSF crsf;

void crsfMain(){
    crsf_channels_t channels;
    crsf_altitude_t altitute = {1000, 0};

    crsf.init(UART_NUM_1, "ZSM");

    crsf_parameter_uint8_t testParamter = {
        .common = {1, 0, 0, CRSF_UINT8, "Test"},
        .value = 50,
        .min = 0,
        .max = 100,
        .unit = "Hz"
    };
    crsf.registerParameter(testParamter.common.dataType, (int*)&testParamter);

    
    crsf_parameter_int16_t testParamter2 = {
        .common = {2, 0, 0, CRSF_INT16, "Test2"},
        .value = 0,
        .min = -10,
        .max = 10,
        .unit = "Hz"
    };
    crsf.registerParameter(testParamter2.common.dataType, (int*)&testParamter2);

    crsf_parameter_float_t testParamter3 = {
        .common = {3, 0, 0, CRSF_FLOAT, "Test3"},
        .value = 0,
        .min = -10,
        .max = 10,
        .def = 0,
        .decPoint = 1,
        .stepSize = 1,
        .unit = "Hz"
    };
    crsf.registerParameter(testParamter3.common.dataType, (int*)&testParamter3);
    

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