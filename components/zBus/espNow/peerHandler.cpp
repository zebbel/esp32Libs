#include "zBusEspNow.h"

static const char *espNowTag = "espnow";

nvs_handle_t zBusEspNow::zBusNVS;
autoConnectPeers_t zBusEspNow::autoConnectPeers;
uint8_t zBusEspNow::autoConnectStatus = ESPNOW_DISCONNECTED;
connectPeers_t zBusEspNow::connectPeers;

/*
enable discover mode
*/
void zBusEspNow::enableDiscoverMode(void *pvParameter){
    espNow_peer_struct_t *peer = (espNow_peer_struct_t*) pvParameter;

    ESP_LOGI(espNowTag, "discovering enabled");
    peer->status = ESPNOW_DISCOVER_MODE;
    discoverMode = true;
    connectPeer = peer;

    while(discoverMode){
        if(peer->status == ESPNOW_DISCONNECTED || peer->status == ESPNOW_CONNECTED) break;
        else if(deInitEspNow) break;
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    if(peer->status == ESPNOW_CONNECTED){
        peer->lastReceived = esp_timer_get_time();
        addEspNowPeer(peer->mac);
        addPeers(peer);
        ESP_LOGI(espNowTag, "discovered: " MACSTR, MAC2STR(peer->mac));
    }else if(peer->status == ESPNOW_DISCONNECTED){
       ESP_LOGI(espNowTag, "discovering aborded"); 
    }

    vTaskDelete(NULL);
}

/*
discover device task
*/
void zBusEspNow::discover(void *pvParameter){
    espNow_peer_struct_t *peer = (espNow_peer_struct_t*) pvParameter;

    ESP_LOGI(espNowTag, "discovering started");
    peer->status = ESPNOW_DISCOVER;
    espnow_data_t buffer;
    buffer.frameType = ESPNOW_FRAME_TYPE_DISCOVER;
    connectPeer = peer;

    while(peer->status != ESPNOW_CONNECTED){
        if(peer->status == ESPNOW_DISCONNECTED) break;
        else if(deInitEspNow) break;
        sendEspNow((uint8_t*)broadcastMAC, &buffer);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    
    if(peer->status == ESPNOW_CONNECTED){
        addAutoConnectPeer(peer);
        peer->lastReceived = esp_timer_get_time();
        saveNvsPeers();
        ESP_LOGI(espNowTag, "discovered: " MACSTR, MAC2STR(peer->mac));
    }else if(peer->status == ESPNOW_DISCONNECTED){
       ESP_LOGI(espNowTag, "discovering aborded"); 
    }

    vTaskDelete(NULL);
}

/*
stop discovering
*/
void zBusEspNow::stopDiscover(espNow_peer_struct_t *peer){
    ESP_LOGI(espNowTag, "stop discovering");
    peer->status = ESPNOW_DISCONNECTED;
    discoverMode = false;
}

void zBusEspNow::autoConnect(void *pvParameter){
    espNow_peer_struct_t *peer = (espNow_peer_struct_t*) pvParameter;

    getNvsPeers();

    espNow_peer_struct_t connectPeers[autoConnectPeers.numPeers];

    for(uint8_t i = 0; i < autoConnectPeers.numPeers; i++){
        ESP_LOGI(espNowTag,  "auto connect to: " MACSTR, MAC2STR(autoConnectPeers.peers[i].mac));
        memcpy(connectPeers[i].mac, autoConnectPeers.peers[i].mac, ESP_NOW_ETH_ALEN);
        xTaskCreate(connect, "espnow_task", 3048, &connectPeers[i], 4, NULL);
    }

    while(autoConnectStatus != ESPNOW_CONNECTED){
        if(deInitEspNow) break;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    for(uint8_t i = 0; i < autoConnectPeers.numPeers; i++){
        if(connectPeers[i].status == ESPNOW_CONNECTED){
            //peer->name = connectPeers[i].name;
            memcpy(peer->name, connectPeers[i].name, ESPNOW_MAX_NAME);
            memcpy(peer->mac, connectPeers[i].mac, ESP_NOW_ETH_ALEN);
            peer->lastReceived = esp_timer_get_time();
            peer->status = ESPNOW_CONNECTED;
            addPeers(peer);
        }
    }

    vTaskDelete(NULL);
}

/*
connect peer task
*/
void zBusEspNow::connect(void *pvParameter){
    espNow_peer_struct_t *peer = (espNow_peer_struct_t*) pvParameter;

    if(peer->status == ESPNOW_DISCONNECTED) peer->status = ESPNOW_CONNECTING;
    else if(peer->status == ESPNOW_CONNECTION_LOST) peer->status = ESPNOW_RECONNECT;

    addPeers(peer);

    ESP_LOGI(espNowTag,  "try to connect to: " MACSTR, MAC2STR(peer->mac));

    espnow_data_t buffer;
    buffer.frameType = ESPNOW_FRAME_TYPE_CONNECT;
    autoConnectPeerInfo info;
    memcpy(info.name, espNowName, ESPNOW_MAX_NAME);
    memcpy(info.mac, peer->mac, ESP_NOW_ETH_ALEN);
    ESP_LOGI(espNowTag, "%s", info.name);

    memcpy(buffer.payload, &info, sizeof(autoConnectPeerInfo));
    while(peer->status != ESPNOW_CONNECTED){
        if(peer->status == ESPNOW_DISCONNECTED || peer->status == ESPNOW_DISCOVER) break;
        else if(peer->status == ESPNOW_CONNECTING && autoConnectStatus == ESPNOW_CONNECTED) break;
        else if(deInitEspNow) break;
        ESP_LOGI(espNowTag,  "send connect to: " MACSTR, MAC2STR(peer->mac));
        sendEspNow((uint8_t*)broadcastMAC, &buffer);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    if(peer->status == ESPNOW_CONNECTED){
        peer->lastReceived = esp_timer_get_time();
        addEspNowPeer(peer->mac);
        autoConnectStatus = ESPNOW_CONNECTED;
        ESP_LOGI(espNowTag,  "connected");
    }else{
        removePeers(peer);
    }

    vTaskDelete(NULL);
}

/*
check if connection is lost
*/
void zBusEspNow::checkConnection(espNow_peer_struct_t *peer){
    // if zBusEspNow is connected and last message received is longer then "ESPNOW_CONNECT_TIMEOUT" ago
    // set status to connection lost and save peer address to reconnectMac
    if(peer->status == ESPNOW_CONNECTED && esp_timer_get_time() > (peer->lastReceived + ESPNOW_CONNECT_TIMEOUT)){
        ESP_LOGI(espNowTag, "Disconnected through timeout");
        peer->status = ESPNOW_CONNECTION_LOST;
    }
}

/*
add peer if not already added
*/
void zBusEspNow::addEspNowPeer(uint8_t *peer_addr){
    if(!esp_now_is_peer_exist(peer_addr)){
        esp_now_peer_info_t *peer = (esp_now_peer_info_t *)malloc(sizeof(esp_now_peer_info_t));
        if (peer == NULL) {
            ESP_LOGE(espNowTag, "Malloc peer information fail");
            vSemaphoreDelete(espnow_queue);
            esp_now_deinit();
        }
        memset(peer, 0, sizeof(esp_now_peer_info_t));
        peer->channel = CONFIG_ESPNOW_CHANNEL;
        peer->ifidx = ESPNOW_WIFI_IF;
        peer->encrypt = true;
        memcpy(peer->lmk, CONFIG_ESPNOW_LMK, ESP_NOW_KEY_LEN);
        memcpy(peer->peer_addr, peer_addr, ESP_NOW_ETH_ALEN);
        esp_now_add_peer(peer);
        free(peer);
        ESP_LOGI(espNowTag, "espNow Peer added: " MACSTR, MAC2STR(peer_addr));
    }
}

void zBusEspNow::addPeers(espNow_peer_struct_t *peer){
    ESP_LOGI(espNowTag, "add peer to peers: " MACSTR, MAC2STR(peer->mac));
    std::string macString = MAC2STRING(peer->mac);
    peers[macString] = peer;

    /*
    std::map<std::string, espNow_peer_struct_t*>::iterator it = peers.begin();
    while (it != peers.end()) {
        std::cout << "Key: " << it->first << "\n";
        ++it;
    }
    ESP_LOGI(espNowTag, "add peer done");
    */
}

void zBusEspNow::removePeers(espNow_peer_struct_t *peer){
    ESP_LOGI(espNowTag, "remove peer from peers: " MACSTR, MAC2STR(peer->mac));
    peers.erase(MAC2STRING(peer->mac));
}

void zBusEspNow::addAutoConnectPeer(espNow_peer_struct_t *peer){
    ESP_LOGI(espNowTag, "add autoconnect name: %s", peer->name);

    uint8_t pos;
    for(pos=0; pos < autoConnectPeers.numPeers; pos++){
        if(strcmp(autoConnectPeers.peers[pos].name, peer->name) == 0) break;
    }
    memcpy(autoConnectPeers.peers[pos].name, peer->name, ESPNOW_MAX_NAME);
    memcpy(autoConnectPeers.peers[pos].mac, peer->mac, ESP_NOW_ETH_ALEN);

    if(pos == autoConnectPeers.numPeers) autoConnectPeers.numPeers++;
}

void zBusEspNow::removeAutoconnectPeer(uint8_t num){
    for(uint8_t pos=num; pos<autoConnectPeers.numPeers; pos++){
        memcpy(autoConnectPeers.peers[pos].name, autoConnectPeers.peers[pos+1].name, ESPNOW_MAX_NAME);
        memcpy(autoConnectPeers.peers[pos].mac, autoConnectPeers.peers[pos+1].mac, ESP_NOW_ETH_ALEN);
    }
    autoConnectPeers.numPeers--;
    saveNvsPeers();
}

void zBusEspNow::addConnectPeer(espNow_peer_struct_t *peer){
    connectPeers.peers[connectPeers.numPeers] = peer;
    connectPeers.numPeers++;
}

void zBusEspNow::getNvsPeers(){
    esp_err_t err = nvs_open("peers", NVS_READONLY, &zBusNVS);
    if(err != ESP_OK){
        ESP_LOGI(espNowTag, "Error (%s) opening NVS handle get nvs peers!", esp_err_to_name(err));
    } else{
        size_t len = sizeof(autoConnectPeers);
        err = nvs_get_blob(zBusNVS, "peers", &autoConnectPeers, &len);
        if(err != ESP_OK){
            ESP_LOGI(espNowTag, "Error (%s) opening NVS handle get nvs peers!", esp_err_to_name(err));
        }
    }
    nvs_close(zBusNVS);
}

void zBusEspNow::saveNvsPeers(){
    esp_err_t err = nvs_open("peers", NVS_READWRITE, &zBusNVS);
    if(err != ESP_OK){
        ESP_LOGI(espNowTag, "Error (%s) opening NVS handle save nvs peers!", esp_err_to_name(err));
    } else{
        err = nvs_set_blob(zBusNVS, "peers", &autoConnectPeers, sizeof(autoConnectPeers));
        if(err != ESP_OK){
            ESP_LOGI(espNowTag, "Error (%s) opening NVS handle save nvs peers!", esp_err_to_name(err));
        }else{
            nvs_commit(zBusNVS);
        }
    }
    nvs_close(zBusNVS);
}

void zBusEspNow::printAutoconnectPeers(){
    for(uint8_t i = 0; i < autoConnectPeers.numPeers; i++){
        ESP_LOGI(espNowTag,  "auto connect to: " MACSTR, MAC2STR(autoConnectPeers.peers[i].mac));
    }
}