#include <stdio.h>
#include "CRSF.h"
#include "byteswap.h"

#define RX_BUF_SIZE 1024    //UART buffer size

// CRC8 lookup table (poly 0xd5)
static uint8_t crc8_table[256] = {0};

CRSF::CRSF(){
}

/**
 * @brief setup CRSF communication
 * 
 * @param uartNumVal: UART number used for crsf communication
 * @param name: the device shows this name over crsf
 */
void CRSF::init(uart_port_t uartNumVal, const char* name){
    generate_CRC(0xd5);

    uartNum = uartNumVal;

    deviceInfo.deviceName = name;
    register_parameter(&rootFolder);

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

    //create semaphore
    xMutex = xSemaphoreCreateMutex();
    //create task
    xTaskCreate(rx_task, "uart_rx_task", 1024*4, this, configMAX_PRIORITIES-1, NULL);
}

/**
 * @brief setup CRSF communication
 * 
 * @param uartNumVal: UART number used for crsf communication
 * @param name: the device shows this name over crsf
 * @param extern_channels: pointer to channel variable
 */
void CRSF::init(uart_port_t uartNumVal, const char* name, crsf_channels_t *extern_channels){
    channels = extern_channels;
    init(uartNumVal, name);
}

/**
 * @brief generate crc table
 */
void CRSF::generate_CRC(uint8_t poly){
    for (int idx=0; idx<256; ++idx)
    {
        uint8_t crc = idx;
        for (int shift=0; shift<8; ++shift){
            crc = (crc << 1) ^ ((crc & 0x80) ? poly : 0);
        }
        crc8_table[idx] = crc & 0xff;
    }
}

/**
 * @brief calculate crc checksum
 * 
 * @param data: pointer to data
 * @param len: number of bytes to calculate checksum
 */
uint8_t CRSF::crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    while (len--){
        crc = crc8_table[crc ^ *data++];
    }

    return crc;
}

/**
 * @brief task that handels incomming uart data
 * 
 * @param pvParameter: pointer to crsf class
 */
void CRSF::rx_task(void *pvParameter){
    CRSF* crsf = reinterpret_cast<CRSF*>(pvParameter); //obtain the instance pointer

    uart_event_t event;
    crsf_broadcast_frame_t frame;

    // sync uart
    ESP_LOGI("crsf", "sync to receiver");
    while(frame.sync != 0xC8) uart_read_bytes(crsf->uartNum, &frame.sync, 1, portMAX_DELAY);
    uart_read_bytes(crsf->uartNum, &frame.len, 1, portMAX_DELAY);
    uart_read_bytes(crsf->uartNum, &frame.type, frame.len, portMAX_DELAY);

    ESP_LOGI("crsf", "synced to receiver");

    while(1){
        //Waiting for UART event.
        if (xQueueReceive(crsf->uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            if (event.type == UART_DATA ) {
                uart_read_bytes(crsf->uartNum, &frame.sync, 2, portMAX_DELAY);

                if(frame.len < 62){
                    uart_read_bytes(crsf->uartNum, &frame.type, frame.len, portMAX_DELAY);

                    if(frame.payload[frame.len-2] == crsf->crc8(&frame.type, frame.len-1)){

                        if(frame.type < 0x27){
                            if(frame.type == CRSF_TYPE_CHANNELS){
                                xSemaphoreTake(crsf->xMutex, portMAX_DELAY);
                                memcpy(crsf->channels, frame.payload, sizeof(crsf_channels_t));
                                xSemaphoreGive(crsf->xMutex);
                            }
                        }else{
                            if(frame.type == CRSF_TYPE_PING && (frame.payload[0] == 0x00 || frame.payload[0] == 0xC8)){
                                //ESP_LOGI("crsf", "respond to ping from: 0x%X", frame.payload[1]);
                                crsf->send_device_info(frame.payload[1], 0xC8);
                            }else if(frame.type == CRSF_FRAMETYPE_PARAMETER_READ && frame.payload[0] == 0xC8){
                                //ESP_LOGI("crsf", "respond to parameter read from: 0x%X, parameter: %i, chunk: %i", frame.payload[1], frame.payload[2], frame.payload[3]);
                                if(frame.payload[2] < crsf->deviceInfo.parameterTotal){
                                    crsf->send_parameter(frame.payload[1], 0xC8, &crsf->parameters[frame.payload[2]]);
                                }
                            }else if(frame.type == CRSF_FRAMETYPE_PARAMETER_WRITE && frame.payload[0] == 0xC8){
                                //ESP_LOGI("crsf", "parameter write: parameter: 0x%X, value: %i", frame.payload[2], frame.payload[3]);
                                if(frame.payload[2] >= 1 && frame.payload[2] < crsf->deviceInfo.parameterTotal){
                                    crsf->handel_parameter_write(frame.payload[1], &crsf->parameters[frame.payload[2]], &frame.payload[3]);
                                }
                            }else{
                                ESP_LOGI("crsf", "type: 0x%X, dest: 0x%X, src: 0x%X", frame.type, frame.payload[0], frame.payload[1]);
                            }
                        }
                    }
                }
            }else if(event.type == UART_FIFO_OVF){
                //ESP_LOGI("crsf", "flush fifo");
                uart_flush_input(crsf->uartNum);
            }
        }
    }
    vTaskDelete(NULL);
}

