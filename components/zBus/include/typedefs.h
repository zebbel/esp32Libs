#include <array>

#define ESPNOW_PAYLOAD_SIZE         30
#define ESPNOW_MAX_NAME             10


typedef enum {
    ESPNOW_DISCONNECTED,
    ESPNOW_CONNECTED,
    ESPNOW_CONNECTION_LOST,
    ESPNOW_CONNECTING,
    ESPNOW_RECONNECT,
    ESPNOW_DISCOVER,
    ESPNOW_DISCOVER_MODE
} connectType_t;

typedef struct{
        char name[ESPNOW_MAX_NAME];
        uint8_t mac[ESP_NOW_ETH_ALEN] = {0};
        uint64_t lastReceived;
        uint8_t sendRetry = 0;
        uint8_t status = ESPNOW_DISCONNECTED;
        QueueHandle_t queue;
} espNow_peer_struct_t;

typedef enum {
    ESPNOW_FRAME_TYPE_DISCOVER,
    ESPNOW_FRAME_TYPE_DISCOVER_RESPONSE,
    ESPNOW_FRAME_TYPE_CONNECT,
    ESPNOW_FRAME_TYPE_CONNECT_RESPONSE,
    ESPNOW_FRAME_TYPE_DATA
} frameType_t;

/* User defined field of ESPNOW data*/
typedef struct {
    frameType_t frameType;                                  //Type of data Frame
    uint16_t crc;                                           //CRC16 value of ESPNOW data.
    uint8_t payload[ESPNOW_PAYLOAD_SIZE] {0};               //Real payload of ESPNOW data.
} __attribute__((packed)) espnow_data_t;

typedef struct{
    char name[ESPNOW_MAX_NAME];
    uint8_t mac[ESP_NOW_ETH_ALEN] = {0};
} autoConnectPeerInfo;

typedef struct{
    uint8_t numPeers = 0;
    autoConnectPeerInfo peers[3];
} autoConnectPeers_t;

typedef struct{
    uint8_t numPeers = 0;
    espNow_peer_struct_t *peers[3];
} connectPeers_t;