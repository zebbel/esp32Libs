#include "CRSF_UART.h"

CRSF_UART::CRSF_UART(){

}

/**
 * @brief init crsf uart communication and start uartRxTask
 * 
 * @param uartNumVal: UART number used for crsf communication
 */
void CRSF_UART::init(uart_port_t uartNumVal){
    uartNum = uartNumVal;

    uart_config_t uart_config;
    uart_config.baud_rate = 420000;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.rx_flow_ctrl_thresh = 0;
    uart_config.source_clk = UART_SCLK_DEFAULT;
    uart_config.flags.allow_pd = 0;
    uart_config.flags.backup_before_sleep = 0;
    uart_param_config(uartNum, &uart_config);

    switch (uartNum){
        case UART_NUM_0:
            uart_set_pin(uartNum, CONFIG_UART0_TX, CONFIG_UART0_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
            break;
        case UART_NUM_1:
            uart_set_pin(uartNum, CONFIG_UART1_TX, CONFIG_UART1_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
            break;
        case UART_NUM_2:
            uart_set_pin(uartNum, CONFIG_UART2_TX, CONFIG_UART2_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
            break;
        default:
            ESP_LOGE("crsf", "uart num error: %d", uartNum);
            break;
    }
    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(uartNum, RX_BUF_SIZE, RX_BUF_SIZE, 10, &uart_queue, 0));

    queue = xQueueCreate(5, 64);

    //create task
    xTaskCreate(uartRxTask, "uart_rx_task", 1024*4, this, configMAX_PRIORITIES-1, NULL);
}

/**
 * @brief task that handels incomming uart data
 * 
 * @param pvParameter: pointer to crsf class
 */
void CRSF_UART::uartRxTask(void *pvParameter){
    CRSF_UART* crsfUart = reinterpret_cast<CRSF_UART*>(pvParameter); //obtain the instance pointer

    uart_event_t event;
    crsf_broadcast_frame_t frame;

    // sync uart
    ESP_LOGI("crsfUart", "sync to receiver");
    while(frame.sync != 0xC8) uart_read_bytes(crsfUart->uartNum, &frame.sync, 1, portMAX_DELAY);
    uart_read_bytes(crsfUart->uartNum, &frame.len, 1, portMAX_DELAY);
    uart_read_bytes(crsfUart->uartNum, &frame.type, frame.len, portMAX_DELAY);

    ESP_LOGI("crsfUart", "synced to receiver");

    while(1){
        //Waiting for UART event.
        if (xQueueReceive(crsfUart->uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            if (event.type == UART_DATA ) {
                uart_read_bytes(crsfUart->uartNum, &frame.sync, 2, portMAX_DELAY);

                if(frame.len < 62){
                    uart_read_bytes(crsfUart->uartNum, &frame.type, frame.len, portMAX_DELAY);

                    if(frame.payload[frame.len-2] == crsfUart->crc8(&crsfUart->crc8_table_crsf[0], &frame.type, frame.len-1)){

                        xQueueSend(crsfUart->queue, &frame, portMAX_DELAY);

                        /*
                        if(frame.type < 0x27){
                            if(frame.type == CRSF_TYPE_CHANNELS){
                                //xSemaphoreTake(crsfUart->xMutex, portMAX_DELAY);
                                memcpy(crsfUart->channels, frame.payload, sizeof(crsf_channels_t));
                                //xSemaphoreGive(crsfUart->xMutex);
                            }
                        }else{
                            if(frame.type == CRSF_TYPE_PING && (frame.payload[0] == 0x00 || frame.payload[0] == 0xC8)){
                                //ESP_LOGI("crsfUart", "respond to ping from: 0x%X", frame.payload[1]);
                                crsfUart->send_device_info(frame.payload[1], 0xC8);
                            }else if(frame.type == CRSF_FRAMETYPE_PARAMETER_READ && frame.payload[0] == 0xC8){
                                //ESP_LOGI("crsfUart", "respond to parameter read from: 0x%X, parameter: %i, chunk: %i", frame.payload[1], frame.payload[2], frame.payload[3]);
                                if(frame.payload[2] < crsfUart->deviceInfo.parameterTotal){
                                    crsfUart->send_parameter(frame.payload[1], 0xC8, &crsfUart->parameters[frame.payload[2]]);
                                }
                            }else if(frame.type == CRSF_FRAMETYPE_PARAMETER_WRITE && frame.payload[0] == 0xC8){
                                //ESP_LOGI("crsfUart", "parameter write: parameter: 0x%X, value: %i", frame.payload[2], frame.payload[3]);
                                if(frame.payload[2] >= 1 && frame.payload[2] < crsfUart->deviceInfo.parameterTotal){
                                    crsfUart->handel_parameter_write(frame.payload[1], &crsfUart->parameters[frame.payload[2]], &frame.payload[3]);
                                }
                            }else{
                                ESP_LOGI("crsfUart", "type: 0x%X, dest: 0x%X, src: 0x%X", frame.type, frame.payload[0], frame.payload[1]);
                            }
                        }
                        */
                    }
                }
            }else if(event.type == UART_FIFO_OVF){
                //ESP_LOGI("crsfUart", "flush fifo");
                uart_flush_input(crsfUart->uartNum);
            }
        }
    }
    vTaskDelete(NULL);
}

/**
 * @brief send data over uart
 * 
 * @param data: pointer to data to be send
 * @param len: number of bytes to be send
 */
void CRSF_UART::send(uint8_t* data, uint8_t len){
    uart_write_bytes(uartNum, data, len);
}