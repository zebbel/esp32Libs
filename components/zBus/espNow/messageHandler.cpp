#include "zBusEspNow.h"

static const char *espNowTag = "espnow";

std::map<std::string, espNow_peer_struct_t*> zBusEspNow::peers;
espNow_peer_struct_t *zBusEspNow::connectPeer;
bool zBusEspNow::discoverMode = false;

/*
zBusEspNow task
handels connecting/reconnecting and discover
*/
void zBusEspNow::task(void *pvParameter){
    zBusEspNow* zbusespnow = reinterpret_cast<zBusEspNow*>(pvParameter); //obtain the instance pointer

    event_recv_cb_t evt;

    //while queue has data
    while(1){
        if(xQueueReceive(zbusespnow->espnow_queue, &evt, 0) == pdTRUE){
            //ESP_LOGI(espNowTag, "received message in queue");

            // if stop flag is set kill task
            if(zbusespnow->deInitEspNow){
                vTaskDelete(NULL);
                break;
            }

            // if event was a broadcast message
            if(IS_BROADCAST_ADDR(evt.des_addr)){
                // handel broadcast message
                //ESP_LOGI(espNowTag, "Received broadcast from: " MACSTR, MAC2STR(evt.src_addr));
                zbusespnow->handleBroadcast(&evt);
            // else if event was not a broadcast message
            }else{
                std::string macString = MAC2STRING(evt.src_addr);
                if(zbusespnow->peers.find(macString) != zbusespnow->peers.end()){
                    espNow_peer_struct_t *peer = zbusespnow->peers[macString];
                    // set status to connected
                    ESP_LOGD(espNowTag, "Connected");
                    peer->status = ESPNOW_CONNECTED;
                }
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/*
handel broadcast messages
*/
void zBusEspNow::handleBroadcast(event_recv_cb_t *recv_cb){
    // switch frame type
    switch(recv_cb->data->frameType){
        // if discover broadcast message was received
        // and discover mode is enabled
        // send response and disable disover mode
        case ESPNOW_FRAME_TYPE_DISCOVER:{
            ESP_LOGI(espNowTag, "received discover frame");
            if(discoverMode){
                sendResponse(recv_cb, ESPNOW_FRAME_TYPE_DISCOVER_RESPONSE);
                memcpy(connectPeer->mac, recv_cb->src_addr, ESP_NOW_ETH_ALEN);
                connectPeer->status = ESPNOW_CONNECTED;
                connectPeer->lastReceived = esp_timer_get_time();
                discoverMode = false;
            }
            break;
        }
        // if connect broadcast message was recieved
        // check if message was for us
        // if so send response
        case ESPNOW_FRAME_TYPE_CONNECT:{
            ESP_LOGI(espNowTag, "received connect frame from:" MACSTR, MAC2STR(recv_cb->src_addr));

            uint8_t mac_base[6] = {0};
            esp_efuse_mac_get_default(mac_base);
            espnow_data_t *data = (espnow_data_t *) recv_cb->data;
            autoConnectPeerInfo peerInfo;
            memcpy(&peerInfo, data->payload, sizeof(autoConnectPeerInfo));
            ESP_LOGI(espNowTag, "base mac:" MACSTR, MAC2STR(mac_base));
            ESP_LOGI(espNowTag, "received mac:" MACSTR, MAC2STR(peerInfo.mac));

            if(IS_MAC_ADDR(peerInfo.mac, mac_base)){
                ESP_LOGI(espNowTag, "mac matches");

                std::string macString = MAC2STRING(recv_cb->src_addr);

                if(peers.find(macString) != peers.end()){
                    espNow_peer_struct_t *peer = peers[macString];
                    peer->status = ESPNOW_CONNECTED;
                    peer->lastReceived = esp_timer_get_time();
                    sendResponse(recv_cb, ESPNOW_FRAME_TYPE_CONNECT_RESPONSE);
                }else{
                    for(uint8_t i=0; i<connectPeers.numPeers; i++){
                        if(strcmp(peerInfo.name, connectPeers.peers[i]->name) == 0){
                            memcpy(connectPeers.peers[i]->mac, recv_cb->src_addr, ESP_NOW_ETH_ALEN);
                            connectPeers.peers[i]->status = ESPNOW_CONNECTED;
                            connectPeers.peers[i]->lastReceived = esp_timer_get_time();
                            addEspNowPeer(recv_cb->src_addr);
                            addPeers(connectPeers.peers[i]);
                            sendResponse(recv_cb, ESPNOW_FRAME_TYPE_CONNECT_RESPONSE);
                            break;
                        }
                    }
                }
            }
            break;
        }
        // if discover response broadcast message was received
        // save mac to auto connect nvs
        // and set status to connected
        case ESPNOW_FRAME_TYPE_DISCOVER_RESPONSE:{
            ESP_LOGI(espNowTag, "received discover response frame");

            memcpy(connectPeer->mac, recv_cb->src_addr, ESP_NOW_ETH_ALEN);
            espnow_data_t *data = (espnow_data_t *) recv_cb->data;
            //connectPeer->name = (const char*)data->payload;
            memcpy(connectPeer->name, data->payload, ESPNOW_MAX_NAME);
            connectPeer->status = ESPNOW_CONNECTED;
            connectPeer->lastReceived = esp_timer_get_time();

            addEspNowPeer(connectPeer->mac);
            addPeers(connectPeer);
            break;
        }
        // if connect or reconnect response broadcast message was received
        // set status to connected
        case ESPNOW_FRAME_TYPE_CONNECT_RESPONSE:{
            ESP_LOGI(espNowTag, "received re/connect response frame");

            std::string macString = MAC2STRING(recv_cb->src_addr);
            if(peers.find(macString) != peers.end()){
                espNow_peer_struct_t *peer = peers[macString];
                peer->status = ESPNOW_CONNECTED;
                peer->lastReceived = esp_timer_get_time();
                autoConnectStatus = ESPNOW_CONNECTED;
            }

            break;
        }
        default:
            break;
    }
}

/*
send broadcast response message
*/
void zBusEspNow::sendResponse(event_recv_cb_t *recv_cb, frameType_t frameType){
    // add peer
    //addEspNowPeer(recv_cb->src_addr);

    // send response message
    espnow_data_t buffer;
    buffer.frameType = frameType;
    memcpy(buffer.payload, espNowName, ESPNOW_MAX_NAME);
    ESP_LOGI(espNowTag, "Send response to: " MACSTR, MAC2STR(recv_cb->src_addr));
    sendEspNow((uint8_t*)broadcastMAC, &buffer);
}