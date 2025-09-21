#pragma once


#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include <string.h>
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <map>

#include "CRSF_UART.h"
#include "CRSF_ESPNOW.h"

//#include "include/typedefDeviceAddress.h"
//#include "include/typedefsBroadcast.h"
//#include "include/typedefsExtended.h"


extern "C"{
    static crsf_parameter_folder_t rootFolder = {.name = "ROOT", .hidden = 0};

    class CRSF: public CRSF_UART, public CRSF_ESPNOW{
        private:
            #define CRSF_MAX_PARAMS 64

            crsf_device_info_t deviceInfo;
            crsf_device_address_t devAddr;

            crsf_parameter_t parameters[CRSF_MAX_PARAMS];
            std::map<const char*, uint8_t> folders = {{"ROOT", 0}};

            bool uartDefined = false;
            bool espNowDefined = false;

            uint8_t* telemetryStartStop;
            void (*externCallback)(crsf_extended_frame_t* frame);

            static void uart_task(void *pvParameter);
            static void esp_now_task(void *pvParameter);
            void handle_broadcast_frame(crsf_broadcast_frame_t* frame);
            void handle_extended_frame(crsf_extended_frame_t* frame);

            void send_broadcast_frame(uint8_t payload_length, crsf_broadcast_type_t type, const void* payload);
            void send_device_info(uint8_t src);
            void send_parameter(uint8_t dest, uint8_t src, crsf_parameter_t* parameter);

            void handle_parameter_common(crsf_extended_frame_t *frame, crsf_parameter_t *parameter, bool hidden);
            void handle_paramter_settings(crsf_extended_frame_t *frame, crsf_parameter_t *parameter);
            void handel_parameter_write(uint8_t dest, crsf_parameter_t *parameter, void *payload);
            void handel_command(crsf_parameter_t *parameter, uint8_t *status, uint8_t dest);
            void handel_direct_command(crsf_extended_frame_t *frame);

            void init(crsf_device_address_t deviceAddress, const char* name);

            void register_parameter(crsf_value_type_e dataType, int *parameterPointer);

        public:
            crsf_channels_t *channels = NULL;

            CRSF();

            void init(crsf_device_address_t deviceAddress, uart_port_t uartNumVal, const char* name);
            void init(crsf_device_address_t deviceAddress, crsf_espNow_role_t espNowRole, const char* name);
            void init(crsf_device_address_t deviceAddress, uart_port_t uartNumVal, crsf_espNow_role_t espNowRole, const char* name);
            void init(crsf_device_address_t deviceAddress, uart_port_t uartNumVal, const char* name, crsf_channels_t *extern_channels);
            void init(crsf_device_address_t deviceAddress, crsf_espNow_role_t espNowRole, const char* name, crsf_channels_t *extern_channels);
            void init(crsf_device_address_t deviceAddress, uart_port_t uartNumVal, crsf_espNow_role_t espNowRole, const char* name, crsf_channels_t *extern_channels);

            void register_callback(void (*callback)(crsf_extended_frame_t* frame));

            uint16_t channel_Mikroseconds(uint16_t value);

            void send_gps(crsf_gps_t* payload);
            void send_gps_time(crsf_gps_time_t* payload);
            void send_gps_extended(crsf_gps_extended_t* payload);
            void send_vertical_speed(crsf_vario_t* payload);
            void send_battery(crsf_battery_t* payload);
            void send_altitute(crsf_altitude_t* payload);
            void send_airspeed(crsf_airspeed_t* payload);
            void send_heartbeat(crsf_heartbeat_t* payload);
            void send_rpm(crsf_rmp_t* payload);
            void send_temp(crsf_temp_t* payload);
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

            void ping_device(crsf_device_address_t dest=CRSF_ADDR_BROADCAST);
            void read_paramter(crsf_device_address_t dest, uint8_t parameterNumber);

            void write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, uint8_t value);
            void write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, int8_t value);
            void write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, uint16_t value);
            void write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, int16_t value);
            void write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, uint32_t value);
            void write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, int32_t value);
            void write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, float value, uint8_t decPoint);

            // direct command
            void set_telemetry_start_stop_var(uint8_t* startStop);
            void send_direct_command(crsf_device_address_t deviceAddr, uint8_t commandType, uint8_t* data);
    };
}