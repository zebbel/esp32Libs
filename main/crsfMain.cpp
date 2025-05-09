#include "esp_timer.h"

#include "crsf.h"

CRSF crsf;

void crsfMain(){
    crsf_channels_t channels;
    crsf_altitude_t altitute = {1000, 0};

    crsf.init(UART_NUM_1, "ZSM");

    crsf_parameter_text_selection_t testParamter = {
        .common = {1, 0, 2, CRSF_TEXT_SELECTION, "Test"},
        .options = "ja;nein",
        .value = 0,
        .min = 0,
        .max = 1,
        .def = 0,
        .unit = "t"
    };

    crsf.registerParameter(testParamter.common.dataType, (int*)&testParamter);

    uint8_t folderList[2] = {1, 0XFF};
    crsf_parameter_folder_t folder = {
        .common = {2, 0, 0, CRSF_FOLDER, "Folder"},
        .children = folderList
    };

    crsf.registerParameter(folder.common.dataType, (int*)&folder);

    crsf_parameter_string_t testParamter2 = {
        .common = {3, 0, 0, CRSF_STRING, "Test2"},
        .value = "Test string",
        .strLen = 100
    };

    crsf.registerParameter(testParamter2.common.dataType, (int*)&testParamter2);
    

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