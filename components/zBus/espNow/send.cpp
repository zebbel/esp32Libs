#include "zBusEspNow.h"

static const char *espNowTag = "espNow";

/*
send frame over zBusEspNow
*/
void zBusEspNow::sendEspNow(uint8_t *peer_addr, espnow_data_t *data){
    data->crc = 0;
    data->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)data, sizeof(espnow_data_t));
    esp_now_send(peer_addr, (uint8_t*)data, sizeof(espnow_data_t));
    //ESP_LOGD(espNowTag, "%s", buffer.c_str());
}

/*
send frame to added peer
*/
void zBusEspNow::sendEspNow(espNow_peer_struct_t *peer, uint8_t *data){
    espnow_data_t buffer;
    buffer.frameType = ESPNOW_FRAME_TYPE_DATA;
    memcpy((buffer.payload + 4), data, data[ZBUS_LEN_POS]);
    sendEspNow(peer->mac, &buffer);
}

void zBusEspNow::sendEspNow(espNow_peer_struct_t * peer, uint8_t dest, uint8_t src, uint8_t type){
    espnow_data_t buffer;
    buffer.frameType = ESPNOW_FRAME_TYPE_DATA;
    buffer.payload[0] = dest;
    buffer.payload[1] = src;
    buffer.payload[2] = type;
    sendEspNow(peer->mac, &buffer);
}

void zBusEspNow::sendEspNow(espNow_peer_struct_t *peer, uint8_t dest, uint8_t src, uint8_t type, uint8_t len, uint8_t *data){
    espnow_data_t buffer;
    buffer.frameType = ESPNOW_FRAME_TYPE_DATA;
    buffer.payload[0] = dest;
    buffer.payload[1] = src;
    buffer.payload[2] = type;
    buffer.payload[3] = len;
    memcpy((buffer.payload + 4), data, len);
    sendEspNow(peer->mac, &buffer);
}

void zBusEspNow::sendEspNow(espNow_peer_struct_t *peer, uint8_t dest, uint8_t src, uint8_t type, int8_t len, int8_t *data){
    espnow_data_t buffer;
    buffer.frameType = ESPNOW_FRAME_TYPE_DATA;
    buffer.payload[0] = dest;
    buffer.payload[1] = src;
    buffer.payload[2] = type;
    buffer.payload[3] = len;
    memcpy((buffer.payload + 4), data, len);
    sendEspNow(peer->mac, &buffer);
}

void zBusEspNow::sendEspNow(espNow_peer_struct_t *peer, uint8_t dest, uint8_t src, uint8_t type, uint16_t *data){
    espnow_data_t buffer;
    buffer.frameType = ESPNOW_FRAME_TYPE_DATA;
    buffer.payload[0] = dest;
    buffer.payload[1] = src;
    buffer.payload[2] = type;
    buffer.payload[3] = sizeof(uint16_t);
    memcpy((buffer.payload + 4), data, sizeof(uint16_t));
    sendEspNow(peer->mac, &buffer);
}
