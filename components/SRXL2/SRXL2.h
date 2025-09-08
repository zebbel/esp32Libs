#ifndef SRXL2_H
#define SRXL2_H

#include "driver/uart.h"
#include <string.h>
#include "esp_log.h"

#include "include/typedefs.h"

#define SRXL2_UART_BUFFER_SIZE 80

#define     SRXL2_POLYNOM                                   0x1021

#define     SRXL2_RCV_ID                                    0x21
#define     SRXL2_ESC_ID                                    0x40

#define     SRXL2_PACKET_HANDSHAKE                          0x21
#define     SRXL2_PACKET_TELEMETRY                          0x80
#define     SRXL2_PACKET_CONTROLL_DATA                      0xCD

#define     SRXL2_CONTROLL_DATA_DELAY                       11
#define     SRXL2_HANDSHAKE_LENGTH                          0x0E
#define     SRXL2_CONTROLL_DATA_UNARMED_LENGTH              0x0E
#define     SRXL2_CONTROLL_DATA_LENGTH                      0x10

#define     SRXL2_TELEMETRY_REQUEST                         5
#define     SRXL2_CONNECTION_TIMEOUT                        1000

#define     SRXL2_SERVO_MIN                                 10912
#define     SRXL2_SERVO_MAX                                 54612
#define     RC_SERVO_MIN                                    1000
#define     RC_SERVO_MAX                                    2000

class SRXL2{
    private:
        uart_port_t uartNum;
        QueueHandle_t uart_queue;

        static void uart_event_task(void *pvParameter);
        
        uint32_t lastTelemetryPacket = 0;
        static void loopTask(void *pvParameter);

        void handleHandshake(uint8_t *buffer);

        void sendPacket(uint8_t *buffer, uint8_t len); 

        uint16_t crcTab16[256];
        void initCrc16Tab();
        uint8_t getChecksum(uint8_t *buffer, uint8_t len);
        bool checkCRC(uint8_t *buffer, uint8_t len);

        srxl2_handshake_packet_t handshakePacket;
        srxl2_channel_data_packet_t controlData;
    
    public:
        SRXL2();
        void init(uart_port_t uartNumVal);

        uint16_t channelValue;

        bool connected = false;
        srxl2_sensors_t sensors;
};

#endif