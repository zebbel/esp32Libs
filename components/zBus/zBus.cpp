#include "zBus.h"

static const char *zbusTAG = "zBus";

zBus::zBus(){
    esp_log_level_set(zbusTAG, ESP_LOG_WARN);
}

void zBus::initEspNow(){
    ESP_LOGD(zbusTAG, "init zbus");
    zBusEspNow::init();
}

void zBus::deInitEspNow(){
    zBusEspNow::deInit();
}

void zBus::initUart(QueueHandle_t uart_queue){
    zBusUart::init(uart_queue);
}

void zBus::connectEspNowDevice(espNow_peer_struct_t *espNowPeer){
    xTaskCreate(zBusEspNow::connect, "espnow_connect_device", 3048, espNowPeer, 4, NULL);
}


void zBus::discoverEspNowDevice(espNow_peer_struct_t *espNowPeer){
    xTaskCreate(zBusEspNow::discover, "espnow_discover", 3048, espNowPeer, 4, NULL);
}

void zBus::enableDiscoverMode(espNow_peer_struct_t *espNowPeer){
    xTaskCreate(zBusEspNow::enableDiscoverMode, "espnow_enable_discover", 3048, espNowPeer, 4, NULL);
}

void zBus::stopDiscover(espNow_peer_struct_t *espNowPeer){
    zBusEspNow::stopDiscover(espNowPeer);
}

void zBus::autoConnect(espNow_peer_struct_t *espNowPeer){
    xTaskCreate(zBusEspNow::autoConnect, "espnow_auto_connect", 3048, espNowPeer, 4, NULL);
}