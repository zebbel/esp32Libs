#include <stdio.h>

#include "zBus.h"
#include <fmt/core.h>


zBus zbus;

QueueHandle_t uartQueue = xQueueCreate(ZBUS_QUEUE_SIZE, ZBUS_UART_BUFFER_SIZE);
uint8_t uartBuffer[ZBUS_UART_BUFFER_SIZE];

zBus_headtracker_t headtracker;
QueueHandle_t headtrackerQueue = xQueueCreate(ZBUS_QUEUE_SIZE, ESPNOW_PAYLOAD_SIZE);
uint8_t headtrackerBuffer[ESPNOW_PAYLOAD_SIZE];


void zBusMain(){
    zbus.initEspNow();


/*
    autoConnectPeerInfo mt12;
    memcpy(mt12.name, "mt12", 5);
    mt12.mac = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1};

    autoConnectPeerInfo xt16;
    memcpy(xt16.name, "xt16", 5);
    xt16.mac = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF2};
*/



/*
    for(uint8_t i = 0; i < autoConnect.numPeers; i++){
        std::cout << autoConnect.peers[i].name << " : " << fmt::format(FMTMACSTR, MAC2STR(autoConnect.peers[i].mac)) << "\n";
    }
*/

/*
    zbus.initUart(uartQueue);
    zbus.initEspNow();

    headtracker.espNowPeer.queue = headtrackerQueue;
    headtracker.name = "headtracker";
    zbus.initEspNowDevice(headtracker.name, &headtracker.espNowPeer);

    headtracker.data.angle.yaw = 5.729;
    headtracker.data.angle.pitch = 3.556;
    headtracker.data.angle.roll = -4.661;

    while(1){
        // uart data received
        if(xQueueReceive(uartQueue, &uartBuffer, 0)){
            // destination device
            switch(uartBuffer[0]){
                // destination radio
                case ZBUS_DEVICE_RADIO:{
                    // frame type
                    switch(uartBuffer[1]){
                        // frame type command
                        case ZBUS_FRAME_TYPE_COMMAND:{
                            switch(uartBuffer[2]){
                                case ZBUS_RADIO_COMMAND_CONFIG:
                                    zbus.sendRadioCommand(ZBUS_DEVICE_RADIO, ZBUS_RADIO_COMMAND_CONFIG);
                                    break;
                            }
                            break;
                        }
                        // frame type data
                        case ZBUS_FRAME_TYPE_DATA:{
                            ESP_LOGI("main", "received radio data from radio");
                            break;
                        }
                    }
                    break;
                }
                // destination headtracker
                case ZBUS_DEVICE_HEADTRACKER:{
                    // frame type
                    switch(uartBuffer[1]){
                        // frame type command
                        case ZBUS_FRAME_TYPE_COMMAND:{
                            switch(uartBuffer[2]){
                                case ZBUS_HEAD_COMMAND_GET_CONFIG:
                                    zbus.sendEspNowCommand(&headtracker.espNowPeer, ZBUS_DEVICE_HEADTRACKER, ZBUS_HEAD_COMMAND_GET_CONFIG);
                                    zbus.sendRadioData(headtracker.espNowPeer.mac);
                                    break;
                                case ZBUS_HEAD_COMMAND_SET_FREQ:
                                    ESP_LOGI("main", "ZBUS_HEAD_COMMAND_SET_FREQ");
                                    break;
                                case ZBUS_HEAD_COMMAND_CENTER:
                                    zbus.sendEspNowCommand(&headtracker.espNowPeer, ZBUS_DEVICE_HEADTRACKER, ZBUS_HEAD_COMMAND_CENTER);
                                    break;
                            }
                            break;
                        }
                        // frame type data
                        case ZBUS_FRAME_TYPE_DATA:{
                            ESP_LOGI("main", "received headtracker data from radio");
                            break;
                        }
                    }
                }
                default:
                    break;
            }
        }

        // headtracker data received
        if(xQueueReceive(headtrackerQueue, &headtrackerBuffer, 0)){
            ESP_LOGI("main", "data received from headtracker");

            //destination device
            switch(headtrackerBuffer[0]){
                //destination radio
                case ZBUS_DEVICE_RADIO:{
                    // frame type
                    switch(headtrackerBuffer[1]){
                        // type data
                        case ZBUS_FRAME_TYPE_DATA:{
                            switch(headtrackerBuffer[2]){
                                case ZBUS_HEAD_DATA_FREQ:
                                    uint32_t freq;
                                    memcpy(&freq, &headtrackerBuffer[3], sizeof(uint32_t));
                                    ESP_LOGI("main", "head freq: %lx", freq);
                                    zbus.sendRadioData(ZBUS_DEVICE_HEADTRACKER, ZBUS_HEAD_DATA_FREQ, headtrackerBuffer[3]);
                                    break;
                                case ZBUS_HEAD_DATA_SCALE:
                                    FusionEuler scale;
                                    memcpy(scale.array, &headtrackerBuffer[3], sizeof(FusionEuler));
                                    ESP_LOGI("main", "scale pitch: %f, roll: %f, yaw: %f", scale.angle.pitch, scale.angle.roll, scale.angle.yaw);
                                    zbus.sendRadioData(ZBUS_DEVICE_HEADTRACKER, ZBUS_HEAD_DATA_SCALE, headtrackerBuffer[3]);
                                    break;
                                case ZBUS_HEAD_DATA_VALUES:
                                    memcpy(headtracker.data.array, &headtrackerBuffer[3], sizeof(headEuler));
                                    zbus.sendRadioData(headtracker.data.angle.yaw, headtracker.data.angle.pitch, headtracker.data.angle.roll);
                                    break;
                            }
                            break;
                        }
                    }
                    break;
                }
            }

        }



        //zbus.sendRadioData(headData.angle.yaw, headData.angle.pitch, headData.angle.roll);
        //zbus.sendRadioCommand(ZBUS_COMMAND_START);
        //zbus.sendUartCommand(ZBUS_COMMAND_START);

        // needs to be here
        // otherwise watchdog triggers...
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
*/
}