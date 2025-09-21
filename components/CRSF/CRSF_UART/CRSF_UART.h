#pragma once

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "CRSF_CRC.h"

#include "../include/typedefDeviceAddress.h"
#include "../include/typedefsBroadcast.h"
#include "../include/typedefsExtended.h"

#define RX_BUF_SIZE 1024    //UART buffer size

extern "C"{
    class CRSF_UART: virtual public CRSF_CRC{
        private:
            uart_port_t uartNum;
            QueueHandle_t uart_queue;

            static void uartRxTask(void *pvParameter);

        public:
            CRSF_UART();
            void init(uart_port_t uartNumVal);
            void send(uint8_t* data, uint8_t len);
            QueueHandle_t queue;
    };
}