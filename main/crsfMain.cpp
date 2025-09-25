#include "esp_timer.h"

#include "CRSF.h"

crsf_direct_command_telemetry_t telemetryStartStop;

typedef struct{
    char deviceName[10];
    uint8_t parameterTotal;
    crsf_device_address_t addr;
} ui_device_info_t;
typedef std::vector<ui_device_info_t> device_info_vector;
device_info_vector devices;

typedef struct{
    uint8_t number;
    char name[10];
} ui_crsf_parameter_t;

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

bool send = true;

crsf_command_status_t commandCallback(){
    ESP_LOGI("testCallback", "juup, %f", floatData);
    return CRSF_COMMAND_READY;
}

/*
void crsfCallback(crsf_extended_frame_t* frame){
    if(frame->type == CRSF_TYPE_DEVICE_INFO){
        ui_device_info_t device;
        uint8_t nameLen = strlen((const char*) frame->payload)+1;

        memcpy(device.deviceName, &frame->payload[0], nameLen);
        device.parameterTotal = frame->payload[nameLen+12];
        device.addr = (crsf_device_address_t) frame->src;
        devices.push_back(device);

        ESP_LOGI("main", "name: %s, parameters total: %d", device.deviceName, device.parameterTotal);

        vTaskDelay(100 / portTICK_PERIOD_MS);
        crsf.read_paramter(CRSF_ADDR_FLIGHT_CONTROLLER, 1);
    }else if(frame->type == CRSF_TYPE_PARAMETER_SETTINGS){
        ui_crsf_parameter_t parameter;
        uint8_t nameLen = strlen((const char*) &frame->payload[4])+1;

        parameter.number = frame->payload[0];
        memcpy(parameter.name, &frame->payload[4], nameLen);

        ESP_LOGI("main", "parameter number: %d, name: %s", parameter.number, parameter.name);
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
        crsf.write_parameter(CRSF_ADDR_FLIGHT_CONTROLLER, 1, (uint8_t) 1);
    }
}
*/

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

        /*
        if(send && crsf.espNowStatus == ESPNOW_STATUS_CONNECTED){
            vTaskDelay(100 / portTICK_PERIOD_MS);
            //crsf.send_direct_command(CRSF_ADDR_FLIGHT_CONTROLLER, CRSF_DIRECT_COMMAND_TELEMETRY, (uint8_t*) &telemetryStartStop);
            //crsf.ping_device();
            send = false;
        }
        */

        /*
        if(ui.crsf.espNowStatus == ESPNOW_STATUS_CONNECTED && xQueueReceive(ui.crsf.CRSF_ESPNOW::queue, &telemetryData, 0) == pdTRUE){
            ESP_LOGI("main", "got data");
        }
        */

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}