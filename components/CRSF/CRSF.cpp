#include <stdio.h>
#include "CRSF.h"
#include "byteswap.h"

#define RX_BUF_SIZE 1024    //UART buffer size

// CRC8 lookup table (poly 0xd5)
static uint8_t crc8_table[256] = {0};

CRSF::CRSF(){
}

void CRSF::init(uart_port_t uartNumVal){
    generate_CRC(0xd5);

    uartNum = uartNumVal;

    uart_config_t uart_config;
    uart_config.baud_rate = 420000;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.rx_flow_ctrl_thresh = 0;
    uart_config.source_clk = UART_SCLK_DEFAULT;
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

// Function to calculate CRC8 checksum
uint8_t CRSF::crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    while (len--){
        crc = crc8_table[crc ^ *data++];
    }

    return crc;
}


void CRSF::rx_task(void *pvParameter){
    CRSF* crsf = reinterpret_cast<CRSF*>(pvParameter); //obtain the instance pointer

    uart_event_t event;
    crsf_broadcast_frame_t frame;

    // sync uart
    ESP_LOGI("crsf", "sync to receiver");
    while(frame.sync != 0xC8) uart_read_bytes(crsf->uartNum, &frame.sync, 1, portMAX_DELAY);
    uart_read_bytes(crsf->uartNum, &frame.len, 1, portMAX_DELAY);
    uart_read_bytes(crsf->uartNum, &frame.type, frame.len, portMAX_DELAY);

    //xQueueReceive(crsf->uart_queue, (void *)&event, (TickType_t)portMAX_DELAY);

    ESP_LOGI("crsf", "synced to receiver");

    while(1){
        //Waiting for UART event.
        if (xQueueReceive(crsf->uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            if (event.type == UART_DATA ) {
                //ESP_LOGI("crsf", "event size: %i", event.size);
                uart_read_bytes(crsf->uartNum, &frame.sync, 2, portMAX_DELAY);
                //uart_read_bytes(crsf->uartNum, &frame.len, 1, portMAX_DELAY);
                //ESP_LOGI("crsf", "sync: 0x%X, len: 0x%X, event size: %i", frame.sync, frame.len, event.size);

                if(frame.len < 62){
                    uart_read_bytes(crsf->uartNum, &frame.type, frame.len, portMAX_DELAY);
                    //ESP_LOGI("crsf", "sync: 0x%X, len: 0x%X, type: 0x%X, crc: 0x%X, crc clac: 0x%X", frame.sync, frame.len, frame.type, frame.payload[frame.len-2], crsf->crc8(&frame.type, frame.len-1));

                    if(frame.payload[frame.len-2] == crsf->crc8(&frame.type, frame.len-1)){
                        //ESP_LOGI("crsf", "sync: 0x%X, len: 0x%X, type: 0x%X", frame.sync, frame.len, frame.type);

                        if(frame.type < 0x27){
                            if(frame.type == CRSF_TYPE_CHANNELS){
                                //ESP_LOGI("crsf", "sync: 0x%X, len: 0x%X, type: 0x%X", frame.sync, frame.len, frame.type);
                                xSemaphoreTake(crsf->xMutex, portMAX_DELAY);
                                crsf->received_channels = *(crsf_channels_t*)frame.payload;
                                xSemaphoreGive(crsf->xMutex);
                                //ESP_LOGI("main", "CH1: %d", crsf->channel_Mikroseconds(crsf->received_channels.ch1));
                            }
                        }else{
                            if(frame.type == CRSF_TYPE_PING){
                                if(frame.payload[0] == 0x00 || frame.payload[0] == 0xC8){
                                    ESP_LOGI("crsf", "respond to ping from: 0x%X", frame.payload[1]);

                                    //crsf_device_info_t info;
                                    //strcpy(info.deviceName, "ZSM");
                                    //info.firmwareId = 0;
                                    //info.hardwareId = 0;
                                    //info.parameterTotal = 0;
                                    //info.parameterVersion = 0;
                                    //info.serialNumber = 0;

                                    //crsf->send_extended_packet(sizeof(info), CRSF_TYPE_DEVICE_INFO, 0xEA, 0xC8, &info);

                                    crsf->send_extended_packet(sizeof(crsf_device_info_t), CRSF_TYPE_DEVICE_INFO, 0xEA, 0xC8, &crsf->deviceInfo);

                                    //uint8_t buffer[] = {0xC8, 0x1C, 0x29, 0xEA, 0xEE, 0x53, 0x49, 0x59, 0x49, 0x20, 0x46, 0x4D, 0x33, 0x30, 0x00, 0x45, 0x4C, 0x52, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0xCA};
                                    //uart_write_bytes(crsf->uartNum, &buffer, sizeof(buffer));
                                }
                            }else{
                                //ESP_LOGI("crsf", "sync: 0x%X, len: 0x%X, type: 0x%X", frame.sync, frame.len, frame.type);
                                //ESP_LOGI("crsf", "put extended frame in queue");
                                xQueueSend(crsf->extendedQueue, &frame.type, 0);
                                //ESP_LOGI("crsf", "sync: 0x%X, len: 0x%X, type: 0x%X, crc: 0x%X, calc crc 0x%X", frame.sync, frame.len, frame.type, frame.payload[frame.len-2], crsf->crc8(&frame.type, frame.len-1));
                                //ESP_LOGI("crsf", "0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X", frame.payload[0], frame.payload[1], frame.payload[2], frame.payload[3], frame.payload[4], frame.payload[5], frame.payload[6], frame.payload[7], frame.payload[8]);
                        
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

//receive uart data frame
void CRSF::receive_channels(crsf_channels_t *channels){
    xSemaphoreTake(xMutex, portMAX_DELAY);
    *channels = received_channels;
    xSemaphoreGive(xMutex);
}

/**
* @brief convert crsf channel value to microseconds
* 
* @param value: channel value
* @returns channel value in microseconds
*/
uint16_t CRSF::channel_Mikroseconds(uint16_t value){
    return 1500.0 + (0.625 * ((float)value - 992.0));
}

/**
 * @brief function sends payload to a destination using uart
 * 
 * @param payload_length length of the payload type
 * @param type type of data contained in payload
 * @param payload pointer to payload of given crsf_type_t
 */
void CRSF::send_packet(uint8_t payload_length, crsf_type_t type, const void* payload){
    uint8_t packet[payload_length+4]; //payload + dest + len + type + crc

    packet[0] = CRSF_SYNC;
    packet[1] = payload_length+2; // size of payload + type + crc
    packet[2] = type;

    memcpy(&packet[3], payload, payload_length);

    //calculate crc
    unsigned char checksum = crc8(&packet[2], payload_length+1);
    
    packet[payload_length+3] = checksum;

    //send frame
    uart_write_bytes(uartNum, &packet, payload_length+4);
}

/**
 * @brief function sends extended packet
 *
 * @param payload_length length of the payload type
 * @param type type of data contained in payload
 * @param dest destination device
 * @param src source device
 * @param payload pointer to payload of given crsf_type_t
 */
void CRSF::send_extended_packet(uint8_t payload_length, crsf_type_t type, uint8_t dest, uint8_t src, const void* payload){
    uint8_t packet[payload_length+6]; //payload + dest + len + type + crc

    packet[0] = CRSF_SYNC;
    packet[1] = payload_length+4; // size of payload + type + crc
    packet[2] = type;
    packet[3] = dest;
    packet[4] = src;

    memcpy(&packet[5], payload, payload_length);

    //calculate crc
    unsigned char checksum = crc8(&packet[2], payload_length+3);
    
    packet[payload_length+5] = checksum;

    //send frame
    uart_write_bytes(uartNum, &packet, payload_length+6);
}

void CRSF::send_gps(crsf_gps_t* payload){
    crsf_gps_t* payload_proc = payload;
    //processed payload
    payload_proc->latitude = __bswap32(payload_proc->latitude);
    payload_proc->longitude = __bswap32(payload_proc->longitude);
    payload_proc->groundspeed = __bswap16(payload_proc->groundspeed);
    payload_proc->heading = __bswap16(payload_proc->heading);
    payload_proc->altitude = __bswap16(payload_proc->altitude);

    send_packet(sizeof(crsf_gps_t), CRSF_TYPE_GPS, payload_proc);
}

void CRSF::send_gps_time(crsf_gps_time_t* payload){
    crsf_gps_time_t* payload_proc = payload;
    //processed payload
    payload_proc->year = __bswap16(payload_proc->year);
    payload_proc->millisecond = __bswap16(payload_proc->millisecond);

    send_packet(sizeof(crsf_gps_time_t), CRSF_TYPE_GPS_TIME, payload_proc);
}

void CRSF::send_gps_extended(crsf_gps_extended_t* payload){
    crsf_gps_extended_t* payload_proc = payload;
    //processed payload
    payload_proc->n_speed = __bswap16(payload_proc->n_speed);
    payload_proc->e_speed = __bswap16(payload_proc->e_speed);
    payload_proc->v_speed = __bswap16(payload_proc->v_speed);
    payload_proc->h_speed_acc = __bswap16(payload_proc->h_speed_acc);
    payload_proc->track_acc = __bswap16(payload_proc->track_acc);
    payload_proc->alt_ellipsoid = __bswap16(payload_proc->alt_ellipsoid);
    payload_proc->h_acc = __bswap16(payload_proc->h_acc);
    payload_proc->v_acc = __bswap16(payload_proc->v_acc);

    send_packet(sizeof(crsf_gps_extended_t), CRSF_TYPE_GPS_EXT, payload_proc);
}

void CRSF::send_vertical_speed(crsf_vario_t* payload){
    crsf_vario_t* payload_proc = 0;
    //processed payload
    payload_proc->v_speed = __bswap16(payload_proc->v_speed);

    send_packet(sizeof(crsf_vario_t), CRSF_TYPE_VARIO, payload_proc);
}

void CRSF::send_battery(crsf_battery_t* payload){
    crsf_battery_t* payload_proc = payload;
    //processed payload
    payload_proc->voltage = __bswap16(payload_proc->voltage);
    payload_proc->current = __bswap16(payload_proc->current);
    payload_proc->capacity_used = __bswap16(payload_proc->capacity_used) << 8;

    send_packet(sizeof(crsf_battery_t), CRSF_TYPE_BATTERY, payload_proc);
}

void CRSF::send_altitute(crsf_altitude_t* payload){
    crsf_altitude_t* payload_proc = payload;
    //processed payload
    payload_proc->altitude = __bswap16(payload_proc->altitude);
    payload_proc->verticalSpeed = __bswap16(payload_proc->verticalSpeed);

    send_packet(sizeof(crsf_altitude_t), CRSF_TYPE_ALTITUDE, payload_proc);
}

void CRSF::send_airspeed(crsf_airspeed_t* payload){
    crsf_airspeed_t* payload_proc = payload;
    //processed payload
    payload_proc->speed = __bswap16(payload_proc->speed);

    send_packet(sizeof(crsf_airspeed_t), CRSF_TYPE_AIRSPEED, payload_proc);
}

void CRSF::send_heartbeat(crsf_heartbeat_t* payload){
    crsf_heartbeat_t* payload_proc = payload;
    //processed payload
    payload_proc->origin_address = __bswap16(payload_proc->origin_address);

    send_packet(sizeof(crsf_heartbeat_t), CRSF_TYPE_HEARTBEAT, payload_proc);
}

void CRSF::send_rpm(crsf_rmp_t* payload, uint8_t numSensors){
    crsf_rmp_t* payload_proc = payload;
    //processed payload
    payload_proc->rpm_source_id = __bswap16(payload_proc->rpm_source_id);

    for(uint8_t i=0; i<numSensors; i++){
        payload_proc->rpm[i] = __bswap32(payload_proc->rpm[i]);
    }

    send_packet(sizeof(crsf_rmp_t), CRSF_TYPE_RPM, payload_proc);
}

void CRSF::send_temp(crsf_temp_t* payload, uint8_t numSensors){
    crsf_temp_t* payload_proc = payload;
    //processed payload
    payload_proc->temp_source_id = __bswap16(payload_proc->temp_source_id);

    for(uint8_t i=0; i<numSensors; i++){
        payload_proc->temperature[i] = __bswap16(payload_proc->temperature[i]);
    }

    //CRSF_send_packet(sizeof(crsf_temp_t), CRSF_TYPE_TEMP, payload_proc);
    send_packet((numSensors*2)+1, CRSF_TYPE_TEMP, payload_proc);
}

void CRSF::send_attitude(crsf_attitude_t* payload){
    crsf_attitude_t* payload_proc = payload;
    //processed payload
    payload_proc->pitch = __bswap16(payload_proc->pitch);
    payload_proc->roll = __bswap16(payload_proc->roll);
    payload_proc->yaw = __bswap16(payload_proc->yaw);

    send_packet(sizeof(crsf_attitude_t), CRSF_TYPE_ATTITUDE, payload_proc);
}