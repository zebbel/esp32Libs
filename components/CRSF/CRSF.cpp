#include <stdio.h>
#include "CRSF.h"
#include "byteswap.h"

CRSF::CRSF(){
}

/**
 * @brief crsf main task
 * 
 * @param pvParameter: pointer to crsf instance
 */
void CRSF::uart_task(void *pvParameter){
    CRSF* crsf = reinterpret_cast<CRSF*>(pvParameter); //obtain the instance pointer

    uint8_t frame[64];
    // frame[0] == sync
    // frame[1] == len
    // frame[2] == type
    // frame[3] == broadcast: payload, extended: dest

    while(1){
        if(xQueueReceive(crsf->CRSF_UART::queue, &frame, 0) == pdTRUE){
            // last broadcast type in crsf is 0x27
            if(frame[2] < 0x27){
                if(crsf->espNowDefined) crsf->CRSF_ESPNOW::send(frame, frame[1] + 2);
                crsf->handle_broadcast_frame((crsf_broadcast_frame_t*) &frame);
            }else if(frame[3] == 0x00 || frame[3] == crsf->devAddr){
                if(crsf->espNowDefined && frame[3] == CRSF_ADDR_BROADCAST) crsf->CRSF_ESPNOW::send(frame, frame[1] + 2);
                crsf->handle_extended_frame((crsf_extended_frame_t*)&frame);
            }else{
                if(crsf->espNowDefined) crsf->CRSF_ESPNOW::send(frame, frame[1] + 2);
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief crsf main task
 * 
 * @param pvParameter: pointer to crsf instance
 */
void CRSF::esp_now_task(void *pvParameter){
    CRSF* crsf = reinterpret_cast<CRSF*>(pvParameter); //obtain the instance pointer

    uint8_t frame[64];
    // frame[0] == sync
    // frame[1] == len
    // frame[2] == type
    // frame[3] == broadcast: payload, extended: dest

    while(1){
        if(crsf->espNowDefined && xQueueReceive(crsf->CRSF_ESPNOW::queue, &frame, 0) == pdTRUE){
            // last broadcast type in crsf is 0x27
            if(frame[2] < 0x27){
                if(crsf->uartDefined) crsf->CRSF_UART::send(frame, frame[1] + 2);
                crsf->handle_broadcast_frame((crsf_broadcast_frame_t*) &frame);
            }else if(frame[3] == CRSF_ADDR_BROADCAST || frame[3] == crsf->devAddr){
                if(crsf->uartDefined && frame[3] == CRSF_ADDR_BROADCAST) crsf->CRSF_UART::send(frame, frame[1] + 2);
                crsf->handle_extended_frame((crsf_extended_frame_t*)&frame);
            }else{
                if(crsf->uartDefined) crsf->CRSF_UART::send(frame, frame[1] + 2);
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief handel crsf broadcast frame
 * 
 * @param frame: pointer to crsf_broadcast_frame_t
 */
void CRSF::handle_broadcast_frame(crsf_broadcast_frame_t* frame){
    if(frame->type == CRSF_TYPE_CHANNELS){
        //xSemaphoreTake(crsf->xMutex, portMAX_DELAY);
        if(channels != NULL) memcpy(channels, frame->payload, sizeof(crsf_channels_t));
        //xSemaphoreGive(crsf->xMutex);
    }
}

/**
 * @brief handel crsf extended frame
 * 
 * @param frame: pointer to crsf_extended_frame_t
 */
void CRSF::handle_extended_frame(crsf_extended_frame_t* frame){
    switch(frame->type){
        case CRSF_TYPE_PING:
            send_device_info(frame->src);
            break;
        
        case CRSF_TYPE_DEVICE_INFO:
        case CRSF_TYPE_PARAMETER_SETTINGS:
            if(externCallback != NULL) externCallback(frame);
            break;

        case CRSF_TYPE_PARAMETER_READ:
            //ESP_LOGI("crsf", "respond to parameter read from: 0x%X, parameter: %i, chunk: %i", frame->src, frame->payload[0], frame->payload[1]);
            if(frame->payload[0] < deviceInfo.parameterTotal) send_parameter(frame->src, devAddr, &parameters[frame->payload[0]]);
            break;

        case CRSF_TYPE_PARAMETER_WRITE:
            //ESP_LOGI("crsf", "parameter write: parameter: 0x%X, value: %i", frame->payload[0], frame->payload[1]);
            if(frame->payload[0] >= 1 && frame->payload[0] < deviceInfo.parameterTotal) handel_parameter_write(frame->src, &parameters[frame->payload[0]], &frame->payload[1]);
            break;

        case CRSF_TYPE_DIRECT_COMMANDS:
            handel_direct_command(frame);
            break;
    
        default:
            ESP_LOGI("crsf", "type: 0x%X, dest: 0x%X, src: 0x%X", frame->type, frame->dest, frame->src);
            break;
    }
}