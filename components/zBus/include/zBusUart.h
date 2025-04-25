#include <stdio.h>
#include <iostream> 
#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include <fmt/core.h>
#include "zBusProtocol.h"

#define ZBUS_UART_BUFFER_SIZE 10

#ifdef CONFIG_UART_NUM_0
    #define ZBUS_UART_NUM UART_NUM_0
#elif CONFIG_UART_NUM_1
    #define ZBUS_UART_NUM UART_NUM_1
#else
    #define ZBUS_UART_NUM UART_NUM_2
#endif

class zBusUart{
private:
    static void uart_event_task(void *pvParameter);
    
//public:
protected:
    QueueHandle_t uart_queue;
    QueueHandle_t extern_queue;

    zBusUart();
    void init(QueueHandle_t queue);

    void sendUart(std::string buffer);

public:
    void sendUart(uint8_t dest, uint8_t src, uint8_t type);
    void sendUart(uint8_t dest, uint8_t src, uint8_t type, bool *data);
    void sendUart(uint8_t dest, uint8_t src, uint8_t type, uint8_t len, uint8_t *data);
    void sendUart(uint8_t dest, uint8_t src, uint8_t type, uint8_t len, uint16_t *data);
    void sendUart(uint8_t dest, uint8_t src, uint8_t type, uint8_t len, int16_t *data);
    void sendUart(uint8_t dest, uint8_t src, uint8_t type, uint8_t len, float *data);
};

