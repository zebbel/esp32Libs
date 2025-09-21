#include "CRSF_ESPNOW.h"

QueueHandle_t CRSF_ESPNOW::espnow_queue;

CRSF_ESPNOW::CRSF_ESPNOW(){
}
    
/**
 * @brief init espNow
 * @param espNowRole: role of espNow
 */
void CRSF_ESPNOW::init(crsf_espNow_role_t espNowRole){
    role = espNowRole;

    watchdogFrame.len = 2;
    watchdogFrame.type = CRSF_TYPE_ESPNOW_WATCHDOG;
    watchdogFrame.payload[0] = crc8((uint8_t*) &crc8_table_crsf, &watchdogFrame.type, watchdogFrame.len-1);


    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    
    // init wifi
    initWifi();
    // init zBusEspNow, if error return with fail
    initEspNow();

    espNowStatus = ESPNOW_STATUS_DISCONNECTED;

    if(role == CRSF_ESPNOW_MASTER) connectDevice();
}

/**
 * @brief deinit espNow
 * @param None
 */
void CRSF_ESPNOW::deInit(){
    vTaskSuspend(esp_now_task);
    // deinit zBusEspNow
    esp_now_deinit();
}

/**
 * @brief init Wifi
 */
void CRSF_ESPNOW::initWifi(){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK( esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_set_channel(CONFIG_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));

    #if CONFIG_ESPNOW_ENABLE_LONG_RANGE
        ESP_ERROR_CHECK( esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
    #endif
}

/**
 * @brief init ESP Now
 */
void CRSF_ESPNOW::initEspNow(){
    /* Initialize ESPNOW and register sending and receiving callback function. */
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(sendCallback));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(recvCallback));
    #if CONFIG_ESPNOW_ENABLE_POWER_SAVE
        ESP_ERROR_CHECK( esp_now_set_wake_window(CONFIG_ESPNOW_WAKE_WINDOW) );
        ESP_ERROR_CHECK( esp_wifi_connectionless_module_set_wake_interval(CONFIG_ESPNOW_WAKE_INTERVAL) );
    #endif

    /* Set primary master key. */
    //ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );

    /* Add broadcast peer information to peer list. */
    esp_now_peer_info_t *peer = (esp_now_peer_info_t *)malloc(sizeof(esp_now_peer_info_t));
    if (peer == NULL) {
        ESP_LOGE("CRSF_ESPNOW", "Malloc peer information fail");
        esp_now_deinit();
    }
    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = CONFIG_ESPNOW_CHANNEL;
    peer->ifidx = ESPNOW_WIFI_IF;
    peer->encrypt = false;
    memcpy(peer->peer_addr, broadcastMAC, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK( esp_now_add_peer(peer) );
    free(peer);

    espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espNowRecvframe));
    queue = xQueueCreate(ESPNOW_QUEUE_SIZE, 250);
    xTaskCreate(task, "espnow_task", 5048, this, 4, &esp_now_task);
}

/**
 * @brief set pointer to function called when ESP Now direct message arrives
 * 
 * @param function: pointer to function
 */
void CRSF_ESPNOW::setEspNowDirectFunction(void (*function)){
    //espNowDirectFunction = function;
}

/**
 * @brief start connectTask
 */
void CRSF_ESPNOW::connectDevice(){
    if(espNowStatus == ESPNOW_STATUS_DISCONNECTED){
        espNowStatus = ESPNOW_STATUS_CONNECTING;
        xTaskCreate(connectTask, "connect_task", 3048, this, 12, NULL);
    }
}

/**
 * @brief sends connect Messages over Broadcast every second till espNowStatus changes
 */
void CRSF_ESPNOW::connectTask(void *pvParameter){
    CRSF_ESPNOW* espnow = reinterpret_cast<CRSF_ESPNOW*>(pvParameter); //obtain the instance pointer

    crsf_broadcast_frame_t frame;
    frame.len = 2;
    frame.type = CRSF_TYPE_ESPNOW_CONNECT;
    frame.payload[0] = espnow->crc8((uint8_t*) &espnow->crc8_table_crsf, &frame.type, frame.len-1);

    while(espnow->espNowStatus == ESPNOW_STATUS_CONNECTING){
        ESP_LOGI("CRSF_ESPNOW", "try to connect to device");
        esp_now_send(espnow->broadcastMAC, (uint8_t *)&frame, 4);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    espnow->watchdogTime = esp_timer_get_time();

    vTaskDelete(NULL);
}

/**
 * @brief ESPNOW sending callback function is called in WiFi task
 */
void CRSF_ESPNOW::sendCallback(const wifi_tx_info_t *sendInfo, esp_now_send_status_t status){

}

/**
 * @brief ESPNOW receiving callback function is called in WiFi task
 */
void CRSF_ESPNOW::recvCallback(const esp_now_recv_info_t *recvInfo, const uint8_t *data, int len){
    espNowRecvframe recvframe;
    memcpy(&recvframe.frame.frame, data, len);
    memcpy(recvframe.deviceMAC, recvInfo->src_addr, ESP_NOW_ETH_ALEN);
    memcpy(&recvframe.frame.rssi, recvInfo->rx_ctrl, 1);

    xQueueSend(espnow_queue, &recvframe, ESPNOW_MAXDELAY);
}

/**
 * @brief CRSF_ESPNOW main task
 * 
 * @param pvParameter: pointer to CRSF_ESPNOW instance
 */
void CRSF_ESPNOW::task(void *pvParameter){
    CRSF_ESPNOW* espnow = reinterpret_cast<CRSF_ESPNOW*>(pvParameter); //obtain the instance pointer

    espNowRecvframe recvframe;

    while(1){
        if(espnow->espNowStatus == ESPNOW_STATUS_CONNECTED){
            espnow->checkWatchdog();
            espnow->sendWatchdog();
        }

        // frame[0] = sync
        // frame[1] = len
        // frame[2] = type

        // broadcast
        // frame[3]-[x] = payload
        // frame[x+1] = crc

        // extended
        // frame[3] = dest
        // frame[4] = src
        // frame[5]-[x] = payload
        // frame[x+1] = crc

        if(xQueueReceive(espnow->espnow_queue, &recvframe, 0) == pdTRUE){
            if(recvframe.frame.frame[recvframe.frame.frame[1]+1] == espnow->crc8((uint8_t*) &espnow->crc8_table_crsf, &recvframe.frame.frame[2], recvframe.frame.frame[1]-1)){
                espnow->watchdogTime = esp_timer_get_time();
                espnow->rssi = recvframe.frame.rssi;

                switch(recvframe.frame.frame[2]){
                    case CRSF_TYPE_ESPNOW_CONNECT:
                        //ESP_LOGI("CRSF_ESPNOW", "received espNow connect message");
                        espnow->handleConnectingframe(&recvframe);;
                        break;

                    case CRSF_TYPE_ESPNOW_WATCHDOG:
                        break;

                    case CRSF_TYPE_ESPNOW_DIRECT:
                        ESP_LOGI("CRSF_ESPNOW", "received espNow direct message");
                        espnow->espNowDirectFunction((uint8_t*) &recvframe.frame.frame);
                        break;
                    
                    default:
                        //ESP_LOGI("CRSF_ESPNOW", "received espNow message");
                        xQueueSend(espnow->queue, &recvframe.frame.frame, ESPNOW_MAXDELAY);
                        break;
                }
            }



            /*
            if(recvframe.frame.frame.type != CRSF_TYPE_ESPNOW_WATCHDOG){
                ESP_LOGI("crsf", "crc: 0x%X, crc calc: 0x%X, len: %d", recvframe.frame.frame.payload[recvframe.frame.frame.len-2], espnow->crc8((uint8_t*) &espnow->crc8_table_crsf, &recvframe.frame.frame.type, recvframe.frame.frame.len-1), recvframe.frame.frame.len);

                if(recvframe.frame.frame.type == CRSF_TYPE_DIRECT_COMMANDS){
                    ESP_LOGI("crsf", "0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X", recvframe.frame.frame.payload[0], recvframe.frame.frame.payload[1], recvframe.frame.frame.payload[2], recvframe.frame.frame.payload[3], recvframe.frame.frame.payload[4], recvframe.frame.frame.payload[5]);
                }
            }

            if(recvframe.frame.frame.payload[recvframe.frame.frame.len-2] == espnow->crc8((uint8_t*) &espnow->crc8_table_crsf, &recvframe.frame.frame.type, recvframe.frame.frame.len-1)){
                espnow->watchdogTime = esp_timer_get_time();
                espnow->rssi = recvframe.frame.rssi;
                
                switch(recvframe.frame.frame.type){
                    case CRSF_TYPE_ESPNOW_CONNECT:
                        ESP_LOGI("CRSF_ESPNOW", "received espNow connect message");
                        espnow->handleConnectingframe(&recvframe);;
                        break;

                    case CRSF_TYPE_ESPNOW_WATCHDOG:
                        break;

                    case CRSF_TYPE_ESPNOW_DIRECT:
                        ESP_LOGI("CRSF_ESPNOW", "received espNow direct message");
                        espnow->espNowDirectFunction((uint8_t*) &recvframe.frame.frame);
                        break;
                    
                    default:
                        ESP_LOGI("CRSF_ESPNOW", "received espNow message");
                        xQueueSend(espnow->queue, &recvframe.frame.frame, ESPNOW_MAXDELAY);
                        break;
                }
            }
            */
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief handel ESP Now connecting frame
 * 
 * @param recvframe: pointer to espNowRecvframe
 */
void CRSF_ESPNOW::handleConnectingframe(espNowRecvframe *recvframe){
    if(espNowStatus == ESPNOW_STATUS_CONNECTING || espNowStatus == ESPNOW_STATUS_DISCONNECTED){
        memcpy(deviceMAC, recvframe->deviceMAC, ESP_NOW_ETH_ALEN);
        if(!esp_now_is_peer_exist(deviceMAC)){
            esp_now_peer_info_t *peer = (esp_now_peer_info_t *)malloc(sizeof(esp_now_peer_info_t));
            memset(peer, 0, sizeof(esp_now_peer_info_t));
            peer->channel = CONFIG_ESPNOW_CHANNEL;
            peer->ifidx = ESPNOW_WIFI_IF;
            peer->encrypt = false;
            memcpy(peer->peer_addr, recvframe->deviceMAC, ESP_NOW_ETH_ALEN);
            ESP_ERROR_CHECK(esp_now_add_peer(peer));
            free(peer);
        }

        espNowStatus = ESPNOW_STATUS_CONNECTED;
        watchdogTime = esp_timer_get_time();

        if(role == CRSF_ESPNOW_SLAVE){
            crsf_broadcast_frame_t frame;
            frame.len = 2;
            frame.type = CRSF_TYPE_ESPNOW_CONNECT;
            frame.payload[0] = crc8((uint8_t*) &crc8_table_crsf, &frame.type, frame.len-1);

            esp_now_send(broadcastMAC, (uint8_t *)&frame, 4);
        }

        ESP_LOGI("CRSF_ESPNOW", "connected to: %02x:%02x:%02x:%02x:%02x:%02x", MAC2STR(recvframe->deviceMAC));
    }
}

/**
 * @brief send a watchdog frame to ESP Now device
 */
void CRSF_ESPNOW::sendWatchdog(){
    esp_now_send(deviceMAC, (uint8_t*) &watchdogFrame, 4);
}

/**
 * @brief check if watchdog is triggered
 */
void CRSF_ESPNOW::checkWatchdog(){
    if(esp_timer_get_time() - watchdogTime > 1000000){
        ESP_LOGI("CRSF_ESPNOW", "watchdog triggered");
        espNowStatus = ESPNOW_STATUS_CONNECTING;
        if(role == CRSF_ESPNOW_MASTER) xTaskCreate(connectTask, "connect_task", 3048, this, 12, NULL);
    }
}

/**
 * @brief send ESP Now frame to device
 * 
 * @param data: pointer to data to be send
 * @param len: number of bytes to be send
 */
void CRSF_ESPNOW::send(uint8_t *data, size_t len){
    esp_now_send(deviceMAC, data, len);
}