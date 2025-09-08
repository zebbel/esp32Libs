#include "zBusUart.h"

static const char *uartTag = "uart";

zBusUart::zBusUart(){
    esp_log_level_set(uartTag, ESP_LOG_WARN);
}

void zBusUart::init(QueueHandle_t queue){
    extern_queue = queue;

    /* Configure parameters of an UART driver,
    * communication pins and install the driver */
    uart_config_t uart_config;
    uart_config.baud_rate = 115200;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.rx_flow_ctrl_thresh = 0;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    //Install UART driver, and get the queue.
    uart_driver_install(ZBUS_UART_NUM, 1024 * 2, 1024 * 2, 20, &uart_queue, 0);
    uart_param_config(ZBUS_UART_NUM, &uart_config);

    //Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(ZBUS_UART_NUM, CONFIG_UART_TX, CONFIG_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 2048, this, 12, NULL);
}

void zBusUart::uart_event_task(void *pvParameter){
    zBusUart* zbusuart = reinterpret_cast<zBusUart*>(pvParameter); //obtain the instance pointer

    uart_event_t event;
    uint8_t buffer[ZBUS_UART_BUFFER_SIZE];

    while(1){
        if(xQueueReceive(zbusuart->uart_queue, &event, 0)) {
            if(event.type == UART_DATA){
                memset(buffer, 0, sizeof(buffer));

                uart_read_bytes(ZBUS_UART_NUM, buffer, event.size, portMAX_DELAY);

                uint8_t queueBuffer[ZBUS_UART_BUFFER_SIZE] = {0};
                uint8_t queueBufferPos = 0;
                for(uint8_t i=0; i<=event.size; i++){
                    if(buffer[i] == ZBUS_END_OF_PACKET){
                        ESP_LOGI(uartTag, "received uart: %s", queueBuffer);
                        xQueueSend(zbusuart->extern_queue, &queueBuffer, portMAX_DELAY);
                        memset(queueBuffer, 0, ZBUS_UART_BUFFER_SIZE);
                        queueBufferPos = 0;
                    }else{
                        queueBuffer[queueBufferPos] = buffer[i];
                        queueBufferPos++;
                    }
                }
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void zBusUart::sendUart(std::string buffer){
    ESP_LOGD(uartTag, "%s", buffer.c_str());

    uart_write_bytes(ZBUS_UART_NUM, buffer.c_str(), buffer.size());
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void zBusUart::sendUart(uint8_t dest, uint8_t src, uint8_t type){
    std::string buffer = fmt::format("{:c};{:c};{:c};1;{:c}", dest, src, type, ZBUS_END_OF_PACKET);
    sendUart(buffer);
}

void zBusUart::sendUart(uint8_t dest, uint8_t src, uint8_t type, bool *data){
    std::string buffer = fmt::format("{:c};{:c};{:c};1;{:d};{:c}", dest, src, type, *data, ZBUS_END_OF_PACKET);
    sendUart(buffer);
}

void zBusUart::sendUart(uint8_t dest, uint8_t src, uint8_t type, uint8_t len, uint8_t *data){
    std::string buffer = fmt::format("{:c};{:c};{:c};{:d}", dest, src, type, len);

    for(uint8_t i=0; i<len; i++){
        buffer.append(fmt::format(";{:d}", *(data+i)));
    }

    buffer.append(fmt::format(";{:c}", ZBUS_END_OF_PACKET));

    sendUart(buffer);
}

void zBusUart::sendUart(uint8_t dest, uint8_t src, uint8_t type, uint8_t len, uint16_t *data){
    std::string buffer = fmt::format("{:c};{:c};{:c};{:d}", dest, src, type, len);

    for(uint8_t i=0; i<len; i++){
        buffer.append(fmt::format(";{:d}", *(data+i)));
    }

    buffer.append(fmt::format(";{:c}", ZBUS_END_OF_PACKET));

    sendUart(buffer);
}

void zBusUart::sendUart(uint8_t dest, uint8_t src, uint8_t type, uint8_t len, int16_t *data){
    std::string buffer = fmt::format("{:c};{:c};{:c};{:d}", dest, src, type, len);

    for(uint8_t i=0; i<len; i++){
        buffer.append(fmt::format(";{:d}", *(data+i)));
    }

    buffer.append(fmt::format(";{:c}", ZBUS_END_OF_PACKET));

    sendUart(buffer);
}

void zBusUart::sendUart(uint8_t dest, uint8_t src, uint8_t type, uint8_t len, float *data){
    std::string buffer = fmt::format("{:c};{:c};{:c};{:d}", dest, src, type, len);

    for(uint8_t i=0; i<len; i++){
        buffer.append(fmt::format(";{:.0f}", *(data+i)));
    }

    buffer.append(fmt::format(";{:c}", ZBUS_END_OF_PACKET));

    sendUart(buffer);
}
