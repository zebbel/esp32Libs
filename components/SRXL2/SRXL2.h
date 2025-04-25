#ifndef SRXL2_H
#define SRXL2_H

#include "driver/uart.h"
#include <string.h>

#include "SRXL2Define.h"
#include "sensor.h"
#include "sBus.h"

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

class SRXL2{
    private:
        Sensor *sensor;
        uart_port_t uartNum;
        QueueHandle_t *extern_queue;
        QueueHandle_t uart_queue;
        QueueHandle_t *channelQueue;

        nvs_handle_t configNVS;
        void getSrxl2Config();

        static void uart_event_task(void *pvParameter);
        
        uint32_t lastTelemetryPacket = 0;
        static void loopTask(void *pvParameter);
        void setControll(uint16_t value);

        void handleHandshake(uint8_t *buffer);
        void handelTelemetry(uint8_t *buffer);

        void sendPacket(uint8_t *buffer, uint8_t len); 

        uint16_t crcTab16[256];
        void initCrc16Tab();
        uint8_t getChecksum(uint8_t *buffer, uint8_t len);
        bool checkCRC(uint8_t *buffer, uint8_t len);

        uint8_t handshakePacket[12] = {0xA6, 0x21, 0x0E, 0x21, 0xFF, 0x0A, 0x00, 0x07, 0x1C, 0xB2, 0x56, 0xCE};
        uint8_t controllDataUnarmed[12] = {0xA6, 0xCD, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        uint8_t controllData[14] = {0xA6, 0xCD, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80};
    
    public:
        SRXL2();
        void init(Sensor *sensorInst, uart_port_t uartNumVal, QueueHandle_t *queue, QueueHandle_t *channelQueueInst);
        void setSrxl2Config();

        uint8_t controlChannel = 0;

        bool connected = false;
        bool failSave = true;
};

#endif