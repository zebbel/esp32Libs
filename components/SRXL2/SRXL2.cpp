#include "SRXL2.h"

static const char *srxl2Tag = "SRXL2";

SRXL2::SRXL2(){

}

void SRXL2::init(uart_port_t uartNumVal){
    uartNum = uartNumVal;

    ESP_LOGI(srxl2Tag, "init SRXL2 on uart%d", uartNum);

    // Configure parameters of an UART driver,
    // communication pins and install the driver
    uart_config_t uart_config;
    uart_config.baud_rate = 115200;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.rx_flow_ctrl_thresh = 0;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    //Install UART driver, and get the queue.
    uart_driver_install(uartNum, 1024 * 2, 1024 * 2, 20, &uart_queue, 0);
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
            ESP_LOGE(srxl2Tag, "uart num error: %d", uartNum);
            break;
    }

    ESP_LOGI(srxl2Tag, "installed SRXL2 on UART %d", uartNum);

    //Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task, "srxl2_uart_event_task", 3048, this, 12, NULL);
    xTaskCreate(loopTask, "srxl2_loop_task", 3048, this, 12, NULL);
}


void SRXL2::uart_event_task(void *pvParameter){
    SRXL2* srxl2 = reinterpret_cast<SRXL2*>(pvParameter); //obtain the instance pointer

    uart_event_t event;
    uint8_t buffer[SRXL2_UART_BUFFER_SIZE];

    while(1){
        if(xQueueReceive(srxl2->uart_queue, &event, 0)) {
            if(event.type == UART_DATA){
                memset(buffer, 0, sizeof(buffer));
                uart_read_bytes(srxl2->uartNum, buffer, event.size, portMAX_DELAY);

                if(srxl2->checkCRC(buffer, buffer[2] + 2)){
                    switch(buffer[1]){
                        case SRXL2_PACKET_HANDSHAKE:{
                            srxl2->handleHandshake(buffer);
                        }break;
                        case SRXL2_PACKET_TELEMETRY:{
                            memcpy(&srxl2->sensors, &buffer[4], sizeof(srxl2_sensors_t));
                            srxl2->lastTelemetryPacket = xTaskGetTickCount();
                        }break;
                    }
	            }
            }
        }  

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void SRXL2::loopTask(void *pvParameter){
    SRXL2* srxl2 = reinterpret_cast<SRXL2*>(pvParameter); //obtain the instance pointer

    uint32_t lastControllData = xTaskGetTickCount();
	uint8_t telemetryRequestCounter = 0;  

    while(1){
        uint32_t timeNow = xTaskGetTickCount();

        if(srxl2->connected){
            if((timeNow > lastControllData + SRXL2_CONTROLL_DATA_DELAY)){
                uint16_t value = (srxl2->channelValue - RC_SERVO_MIN) * (SRXL2_SERVO_MAX - SRXL2_SERVO_MIN) / (RC_SERVO_MAX - RC_SERVO_MIN) + SRXL2_SERVO_MIN;
                memcpy(&srxl2->controlData, &value, 2);

                if(telemetryRequestCounter++ > SRXL2_TELEMETRY_REQUEST){
                    //srxl2->controllData[4] = SRXL2_ESC_ID;
                    srxl2->controlData.replyId = SRXL2_ESC_ID;
                    telemetryRequestCounter = 0;
                }else{
                    //srxl2->controllData[4] = 0x00;
                    srxl2->controlData.replyId = 0x00;
                }
                srxl2->sendPacket(&srxl2->controlData.manifacturerId, SRXL2_CONTROLL_DATA_LENGTH);

                lastControllData = timeNow;                                                                                                                             // save time we last send a controll data packet

                if(timeNow >= srxl2->lastTelemetryPacket + SRXL2_CONNECTION_TIMEOUT){                                                                                                // if last time a telemetry packet is greater then CONNECTION_TIMEOUT ESC is not connected anymore
                    srxl2->connected = false;
                }
            }
        }else{                                                                                                                                                          // else ESC is not connected
            srxl2->lastTelemetryPacket = timeNow;                                                                                                                              // set lastTelemetryPacket to acuall time
        }
    }
}


void SRXL2::handleHandshake(uint8_t *buffer){
    // if the packet is from the ESC (just in case even no other device is connected :-D)
    if(buffer[3] == SRXL2_ESC_ID){
        sendPacket(&handshakePacket.manifacturerId, SRXL2_HANDSHAKE_LENGTH);
        // ensure we don't send a controll data packet immediately after handshake packet
        vTaskDelay(SRXL2_CONTROLL_DATA_DELAY / portTICK_PERIOD_MS);
        
        connected = true;
    }
}

void SRXL2::sendPacket(uint8_t *buffer, uint8_t len){
    uint16_t checksum = getChecksum(buffer, len);
    memcpy(&buffer[len-2], &checksum, 2);
    uart_write_bytes(uartNum, buffer, len); 
}

void SRXL2::initCrc16Tab(){
	uint16_t crc;
	uint16_t c;

	for(uint16_t i=0; i<256; i++){
		crc = 0;
		c = i << 8;

		for(uint16_t j=0; j<8; j++){
			if((crc ^ c) & 0x8000 ) crc = ( crc << 1 ) ^ SRXL2_POLYNOM;
			else crc = crc << 1;
            
			c = c << 1;
		}
		crcTab16[i] = crc;
	}
}

uint8_t SRXL2::getChecksum(uint8_t *buffer, uint8_t len){
    uint16_t crc = 0x0000;

	for (uint8_t i=0; i<len; i++) {
		crc = (crc << 8) ^ crcTab16[((crc >> 8) ^ (uint16_t) buffer[i]) & 0x00FF];
	}

	return crc;
}

bool SRXL2::checkCRC(uint8_t *buffer, uint8_t len){
	return getChecksum(buffer, len) == ((buffer[buffer[2]-2] << 8) + buffer[buffer[2]-1]);
}