#include "esp_timer.h"

#include "crsf.h"

CRSF crsf;
crsf_channels_t channels;
int16_t temperatur = 255;

uint8_t uint8Data = 51;
int8_t int8Data = 51;
uint16_t uint16Data = 51;
int16_t int16Data = 51;
uint32_t uint32Data = 51;
int32_t int32Data = 51;
float floatData = 6.4;
uint8_t textSelelctData = 0;

crsf_command_status_t commandCallback(){
    ESP_LOGI("testCallback", "juup, %f", floatData);
    return CRSF_COMMAND_READY;
}

void init_parameter(){
    crsf_parameter_uint8_t uint8Parameter = {
        .name = "uint8 ",
        .value = &uint8Data,
        .min = 0,
        .max = 100,
        .unit = "Hz",
        .hidden = 0
    };
    crsf.register_parameter(&uint8Parameter);

    crsf_parameter_int8_t int8Parameter = {
        .name = "int8  ",
        .value = &int8Data,
        .min = -100,
        .max = 100,
        .unit = "Hz",
        .hidden = 0
    };
    crsf.register_parameter(&int8Parameter);

    crsf_parameter_uint16_t uint16Parameter = {
        .name = "uint16",
        .value = &uint16Data,
        .min = 0,
        .max = 100,
        .unit = "Hz",
        .hidden = 0
    };
    crsf.register_parameter(&uint16Parameter);

    crsf_parameter_int16_t int16Parameter = {
        .name = "int16 ",
        .value = &int16Data,
        .min = -100,
        .max = 100,
        .unit = "Hz",
        .hidden = 0
    };
    crsf.register_parameter(&int16Parameter);

    /* expressLRS lua doesn´t support 32bit
    crsf_parameter_uint32_t uint32Parameter = {
        .name = "uint32",
        .value = &uint32Data,
        .min = 0,
        .max = 100,
        .unit = "Hz",
        .hidden = 0
    };
    crsf.register_parameter(&uint32Parameter);

    crsf_parameter_int32_t int32Parameter = {
        .name = "int32",
        .value = &int32Data,
        .min = -100,
        .max = 100,
        .unit = "Hz",
        .hidden = 0
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
        .unit = "Hz",
        .hidden = 0
    };
    crsf.register_parameter(&floatParameter);

    crsf_parameter_text_selection_t textSelectParamter = {
        .name = "choose",
        .options = "show;hide",
        .value = &textSelelctData,
        .min = 0,
        .max = 1,
        .def = 0,
        .unit = " ",
        .hidden = 0
    };
    crsf.register_parameter(&textSelectParamter);

    crsf_parameter_folder_t folderParameter = {
        .name = "Folder",
        .hidden = 0
    };
    crsf.register_parameter(&folderParameter);

    crsf_parameter_string_t stringParameter = {
        .name = "String",
        .value = "Test",
        .strLen = 30,
        .hidden = 0
    };
    crsf.register_parameter(&stringParameter, folderParameter);

    crsf_parameter_info_t infoParameter = {
        .name = "Info  ",
        .info = "juup",
        .hidden = 0
    };
    crsf.register_parameter(&infoParameter, folderParameter);

    crsf_parameter_folder_t subFolderParameter = {
        .name = "Sub Folder",
        .hidden = 0
    };
    crsf.register_parameter(&subFolderParameter, folderParameter);

    crsf_parameter_command_t commandParameter = {
        .name = "Print ",
        .status = CRSF_COMMAND_READY,
        .timeout = 100,
        .info = "print Data",
        .callback = &commandCallback,
        .hidden = 0
    };
    crsf.register_parameter(&commandParameter, subFolderParameter);
}

void crsfMain(){
    crsf_channels_t channels;
    crsf_temp_t tempSensor = {
        .temp_source_id = 0,
        .temperature = {255},
        .num_sensors = 1
    };

    crsf.init(UART_NUM_1, "ZSM", &channels);

    //init_parameter();

    uint64_t lastSend = esp_timer_get_time();

    while(1){
        ESP_LOGI("main", "CH1: %i", crsf.channel_Mikroseconds(channels.ch1));

        //if((lastSend + 1000) < esp_timer_get_time()){
        //    lastSend = esp_timer_get_time();
        //    crsf.send_temp(&tempSensor);
        //}

        //folderParameter.hidden = textSelelctData;

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}