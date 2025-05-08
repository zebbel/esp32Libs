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

#include "include/typedefsBroadcast.h"
#include "include/typedefsExtended.h"

class CRSF{
    private:
        SemaphoreHandle_t xMutex;

        uart_port_t uartNum;
        QueueHandle_t uart_queue;

        crsf_channels_t received_channels;

        void generate_CRC(uint8_t poly);
        uint8_t crc8(const uint8_t *data, uint8_t len);
        static void rx_task(void *pvParameter);
        void send_broadcast_packet(uint8_t payload_length, crsf_broadcast_type_t type, const void* payload);
        void send_extended_packet(crsf_extended_type_t type, uint8_t dest, uint8_t src, void* payload);

    public:
        crsf_device_info_t *deviceInfo;
        QueueHandle_t extendedQueue = xQueueCreate(5, sizeof(crsf_extended_data_t));

        CRSF();

        /**
         * @brief setup CRSF communication
         * 
         * @param uartNumVal: UARt number used for crsf communication
         */
        void init(uart_port_t uartNumVal);

        void receive_channels(crsf_channels_t *channels);
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
};

#endif