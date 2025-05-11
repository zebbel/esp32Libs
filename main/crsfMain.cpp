#include "esp_timer.h"

#include "crsf.h"

CRSF crsf;

uint8_t uint8Data = 51;
int8_t int8Data = 51;
uint16_t uint16Data = 51;
int16_t int16Data = 51;
uint32_t uint32Data = 51;
int32_t int32Data = 51;
float floatData = 6.4;

crsf_command_status_t testCallback(){
    //ESP_LOGI("testCallback", "juup, %f", testData);
    return CRSF_COMMAND_READY;
}

void crsfMain(){
    crsf_channels_t channels;
    crsf_altitude_t altitute = {1000, 0};

    crsf.init(UART_NUM_1, "ZSM");

    crsf_parameter_uint8_t uint8Parameter = {
        .name = "uint8 ",
        .value = &uint8Data,
        .min = 0,
        .max = 100,
        .unit = "Hz"
    };
    crsf.register_parameter(&uint8Parameter);

    crsf_parameter_int8_t int8Parameter = {
        .name = "int8  ",
        .value = &int8Data,
        .min = -100,
        .max = 100,
        .unit = "Hz"
    };
    crsf.register_parameter(&int8Parameter);

    crsf_parameter_uint16_t uint16Parameter = {
        .name = "uint16",
        .value = &uint16Data,
        .min = 0,
        .max = 100,
        .unit = "Hz"
    };
    crsf.register_parameter(&uint16Parameter);

    crsf_parameter_int16_t int16Parameter = {
        .name = "int16 ",
        .value = &int16Data,
        .min = -100,
        .max = 100,
        .unit = "Hz"
    };
    crsf.register_parameter(&int16Parameter);

    /* expressLRS lua doesn´t support 32bit
    crsf_parameter_uint32_t uint32Parameter = {
        .name = "uint32",
        .value = &uint32Data,
        .min = 0,
        .max = 100,
        .unit = "Hz"
    };
    crsf.register_parameter(&uint32Parameter);

    crsf_parameter_int32_t int32Parameter = {
        .name = "int32",
        .value = &int32Data,
        .min = -100,
        .max = 100,
        .unit = "Hz"
    };
    crsf.register_parameter(&int32Parameter);
    */

    crsf_parameter_float_t floatParameter = {
        .name = "float ",
        .value = &floatData,
        .min = -100,
        .max = 100,
        .def = 0,
        .decPoint = 1,
        .stepSize = 1,
        .unit = "Hz"
    };
    crsf.register_parameter(&floatParameter);

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