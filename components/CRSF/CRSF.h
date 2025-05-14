#ifndef CRSF_H
#define CRSF_H

#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include <string.h>
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <map>

#include "include/typedefsBroadcast.h"
#include "include/typedefsExtended.h"

static crsf_parameter_folder_t rootFolder = {.name = "ROOT", .hidden = 0};

class CRSF{
    private:
        #define CRSF_MAX_PARAMS 64

        SemaphoreHandle_t xMutex;

        crsf_device_info_t deviceInfo;

        uart_port_t uartNum;
        QueueHandle_t uart_queue;

        crsf_parameter_t parameters[CRSF_MAX_PARAMS];
        std::map<const char*, uint8_t> folders = {{"ROOT", 0}};

        void generate_CRC(uint8_t poly);
        uint8_t crc8(const uint8_t *data, uint8_t len);
        static void rx_task(void *pvParameter);
        void send_broadcast_packet(uint8_t payload_length, crsf_broadcast_type_t type, const void* payload);
        void send_device_info(uint8_t dest, uint8_t src);
        void send_parameter(uint8_t dest, uint8_t src, crsf_parameter_t* parameter);

        void handle_parameter_common(crsf_extended_t *packet, crsf_parameter_t *parameter, bool hidden);
        void handle_paramter_settings(crsf_extended_t *packet, crsf_parameter_t *parameter);
        void handel_parameter_write(uint8_t dest, crsf_parameter_t *parameter, void *payload);
        void handel_command(crsf_parameter_t *parameter, uint8_t *status, uint8_t dest);

        void register_parameter(crsf_value_type_e dataType, int *parameterPointer);

    public:
        crsf_channels_t *channels;

        CRSF();

        void init(uart_port_t uartNumVal, const char* name);
        void init(uart_port_t uartNumVal, const char* name, crsf_channels_t *extern_channels);

        uint16_t channel_Mikroseconds(uint16_t value);

        void send_gps(crsf_gps_t* payload);
        void send_gps_time(crsf_gps_time_t* payload);
        void send_gps_extended(crsf_gps_extended_t* payload);
        void send_vertical_speed(crsf_vario_t* payload);
        void send_battery(crsf_battery_t* payload);
        void send_altitute(crsf_altitude_t* payload);
        void send_airspeed(crsf_airspeed_t* payload);
        void send_heartbeat(crsf_heartbeat_t* payload);
        void send_rpm(crsf_rmp_t* payload, uint8_t numSensors);
        void send_temp(crsf_temp_t* payload, uint8_t numSensors);
        void send_attitude(crsf_attitude_t* payload);

        void register_parameter(crsf_parameter_uint8_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_int8_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_uint16_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_int16_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_uint32_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_int32_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_float_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_text_selection_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_string_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_folder_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_info_t *parameter, crsf_parameter_folder_t folder=rootFolder);
        void register_parameter(crsf_parameter_command_t *parameter, crsf_parameter_folder_t folder=rootFolder);
};

#endif