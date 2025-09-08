#include "zBusEspNow.h"

static const char *espNowTag = "espnow";

char zBusEspNow::espNowName[] = CONFIG_DEVICE_NAME;
QueueHandle_t zBusEspNow::espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(event_recv_cb_t));
bool zBusEspNow::deInitEspNow = false;

/*
constructor
*/
zBusEspNow::zBusEspNow(){
    esp_log_level_set(espNowTag, ESP_LOG_WARN);
}

/*
init zBusEspNow
*/
void zBusEspNow::init(){
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
}

/*
deinit zBusEspNow
*/
void zBusEspNow::deInit(){
    // delete zBusEspNow Task
    deInitEspNow = true;
    // deinit zBusEspNow
    esp_now_deinit();
}

/*
init wifi
*/
void zBusEspNow::initWifi(){
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

/*
init zBusEspNow
*/
void zBusEspNow::initEspNow(){
    // if queue is not created exit with error
    if (espnow_queue == NULL) {
        ESP_LOGE(espNowTag, "Create mutex fail");
    }

    /* Initialize ESPNOW and register sending and receiving callback function. */
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(sendCallback));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(recvCallback));
    #if CONFIG_ESPNOW_ENABLE_POWER_SAVE
        ESP_ERROR_CHECK( esp_now_set_wake_window(CONFIG_ESPNOW_WAKE_WINDOW) );
        ESP_ERROR_CHECK( esp_wifi_connectionless_module_set_wake_interval(CONFIG_ESPNOW_WAKE_INTERVAL) );
    #endif

    /* Set primary master key. */
    ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );

    /* Add broadcast peer information to peer list. */
    esp_now_peer_info_t *peer = (esp_now_peer_info_t *)malloc(sizeof(esp_now_peer_info_t));
    if (peer == NULL) {
        ESP_LOGE(espNowTag, "Malloc peer information fail");
        vSemaphoreDelete(espnow_queue);
        esp_now_deinit();
    }
    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = CONFIG_ESPNOW_CHANNEL;
    peer->ifidx = ESPNOW_WIFI_IF;
    peer->encrypt = false;
    memcpy(peer->peer_addr, broadcastMAC, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK( esp_now_add_peer(peer) );
    free(peer);

    /* create zBusEspNow task */
    xTaskCreate(task, "espnow_task", 3048, this, 4, NULL);
}

void zBusEspNow::getEspNowName(){
    esp_err_t err = nvs_open("espNow", NVS_READONLY, &zBusNVS);
    if(err != ESP_OK){
        ESP_LOGI(espNowTag, "Error (%s) opening NVS handle get espNow name!", esp_err_to_name(err));
    } else{
        //size_t len = sizeof(autoConnectPeers);
        //err = nvs_get_blob(zBusNVS, "name", &autoConnectPeers, &len);
        size_t len = ESPNOW_MAX_NAME;
        err = nvs_get_str(zBusNVS, "name", espNowName, &len);
        if(err != ESP_OK){
            ESP_LOGI(espNowTag, "Error (%s) opening NVS handle get espNow name!", esp_err_to_name(err));
        }
    }
    nvs_close(zBusNVS);
}

void zBusEspNow::setEspNowName(){
    esp_err_t err = nvs_open("espNow", NVS_READWRITE, &zBusNVS);
    if(err != ESP_OK){
        ESP_LOGI(espNowTag, "Error (%s) opening NVS handle set espNow name!", esp_err_to_name(err));
    } else{
        //err = nvs_set_blob(zBusNVS, "name", &autoConnectPeers, sizeof(autoConnectPeers));
        err = nvs_set_str(zBusNVS, "name", (const char*)&espNowName);
        if(err != ESP_OK){
            ESP_LOGI(espNowTag, "Error (%s) opening NVS handle espNow name!", esp_err_to_name(err));
        }else{
            nvs_commit(zBusNVS);
        }
    }
    nvs_close(zBusNVS);
}