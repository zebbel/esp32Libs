#include "zBusEspNow.h"

static const char *espNowTag = "espnow";

/* ESPNOW sending or receiving callback function is called in WiFi task.
 * Users should not do lengthy operations from this task. Instead, post
 * necessary data to a queue and handle it from a lower priority task. */
void zBusEspNow::sendCallback(const wifi_tx_info_t *info, esp_now_send_status_t sendStatus){
    std::string macString = MAC2STRING(info->des_addr);

    // if zBusEspNow is connected and send messaged doesn´t reached the peer
    if(peers.find(macString) != peers.end()){
        espNow_peer_struct_t *peer = peers[macString];

        if(peer->status == ESPNOW_CONNECTED && sendStatus){
            peer->sendRetry++;
            // if 3 messages where not able to be delivered set disconnect status
            if( peer->sendRetry >= 3){
                ESP_LOGI(espNowTag, "Disconnected through retry");
                peer->status = ESPNOW_CONNECTION_LOST;
            }
        // else message has reached the peer, reset sendRety to zero
        }else{
            peer->sendRetry = 0;
            peer->lastReceived = esp_timer_get_time();
        }
    }
}

/*
callback gets called when zBusEspNow message is received
Users should not do lengthy operations from this task. Instead, post
necessary data to a queue and handle it from a lower priority task.
*/
void zBusEspNow::recvCallback(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len){
    // create event from callback data
    event_recv_cb_t evt;
    memcpy(evt.src_addr, recv_info->src_addr, ESP_NOW_ETH_ALEN);
    memcpy(evt.des_addr, recv_info->des_addr, ESP_NOW_ETH_ALEN);
    evt.data = (espnow_data_t*)malloc(len);
    memcpy(evt.data, data, len);
    evt.data_len = len;

    // if crc check is ok
    if(checkCRC(evt.data, evt.data_len)){
        std::string macString = MAC2STRING(evt.src_addr);

        //ESP_LOGI(espNowTag, "received from: " MACSTR, MAC2STR(evt.src_addr));
        //ESP_LOGI(espNowTag, "frame type: %d", evt.data->frameType);

        // if frame type is data and peer is known
        if(evt.data->frameType == ESPNOW_FRAME_TYPE_DATA && peers.find(macString) != peers.end()){
            //ESP_LOGI(espNowTag, "mac string found");
            espNow_peer_struct_t *peer = peers[macString];

            peer->lastReceived = esp_timer_get_time();
            peer->status = ESPNOW_CONNECTED;

            // if extern queue is set
            if(peer->queue != NULL){
                // send payload to queue
                if (xQueueSend(peer->queue, &evt.data->payload, ESPNOW_MAXDELAY) != pdTRUE) {
                    ESP_LOGW(espNowTag, "Send extern queue fail");
                }
            }
        // else handel received event in task
        }else{
            //ESP_LOGI(espNowTag, "put message in queue for task");

            if (xQueueSend(espnow_queue, &evt, ESPNOW_MAXDELAY) != pdTRUE) {
                ESP_LOGW(espNowTag, "Send receive queue fail");
            }
        }
    }

    //free(evt.data);
}

/* 
check if crc matches.
*/
int zBusEspNow::checkCRC(espnow_data_t *data, uint16_t data_len){
    uint16_t crc;
    crc = data->crc;
    data->crc = 0;
    uint16_t crcCalc = esp_crc16_le(UINT16_MAX, (uint8_t const *)data, sizeof(espnow_data_t));

    //ESP_LOGD(espNowTag, "crc: %X, crcCalc: %X", crc, crcCalc);

    if (crc == crcCalc) return 1;
    return 0;
}