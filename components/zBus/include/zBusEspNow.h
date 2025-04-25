#ifndef ESPNOW_H
#define ESPNOW_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_now.h"
#include "esp_mac.h"
#include "esp_crc.h"
#include "esp_timer.h"
#include <fmt/core.h>

#include "typedefs.h"
#include "zBusProtocol.h"


/* ESPNOW can work in both station and softap mode. It is configured in menuconfig. */
#if CONFIG_ESPNOW_WIFI_MODE_STATION
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   WIFI_IF_STA
#else
#define ESPNOW_WIFI_MODE WIFI_MODE_AP
#define ESPNOW_WIFI_IF   WIFI_IF_PA
#endif

#define ESPNOW_QUEUE_SIZE           6
#define ESPNOW_MAXDELAY             512

#define ESPNOW_CONNECT_TIMEOUT      5000000

#define IS_BROADCAST_ADDR(addr) (memcmp(addr, broadcastMAC, ESP_NOW_ETH_ALEN) == 0)
#define IS_MAC_ADDR(addr1, addr2) (memcmp(addr1, addr2, ESP_NOW_ETH_ALEN) == 0)
#define MAC2STRING(addr) fmt::format(FMTMACSTR, MAC2STR(addr))
#define FMTMACSTR "{:x}:{:x}:{:x}:{:x}:{:x}:{:x}"

class zBusEspNow{

    private:
        typedef struct {
            uint8_t src_addr[ESP_NOW_ETH_ALEN];
            uint8_t des_addr[ESP_NOW_ETH_ALEN];
            espnow_data_t *data;
            int data_len;
        } event_recv_cb_t;

        static bool deInitEspNow;                                                       // task gets deleted if deInitEspNow is true
        static QueueHandle_t espnow_queue;                                              // data queue for task
        
    public:
        static char espNowName[];
        zBusEspNow();
        void init();
        void deInit();
    private:
        void initWifi();
        void initEspNow();
    public:
        void getEspNowName();
        void setEspNowName();

    private:
        static std::map<std::string, espNow_peer_struct_t*> peers;
        static void sendCallback(const uint8_t *mac_addr, esp_now_send_status_t sendStatus);
        static void recvCallback(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
        static int checkCRC(espnow_data_t *data, uint16_t data_len);

        static bool discoverMode;
        static void task(void *pvParameter);
        void handleBroadcast(event_recv_cb_t *recv_cb);
        void sendResponse(event_recv_cb_t *recv_cb, frameType_t frameType);

        static nvs_handle_t zBusNVS;
    public:
        static autoConnectPeers_t autoConnectPeers;
    private:
        static espNow_peer_struct_t *connectPeer;
        static uint8_t autoConnectStatus;
        static connectPeers_t connectPeers;
    protected:
        static void enableDiscoverMode(void *pvParameter);
        static void discover(void *pvParameter);
        static void stopDiscover(espNow_peer_struct_t *peer);
        static void autoConnect(void *pvParameter);
        static void connect(void *pvParameter);
    public:
        void checkConnection(espNow_peer_struct_t *peer);
    private:
        static void addEspNowPeer(uint8_t *peer_addr);
        static void addPeers(espNow_peer_struct_t *peer);
        static void removePeers(espNow_peer_struct_t *peer);
        static void addAutoConnectPeer(espNow_peer_struct_t *peer);
    public:
        static void removeAutoconnectPeer(uint8_t num);
        void addConnectPeer(espNow_peer_struct_t *peer);
    private:
        static void getNvsPeers();
    public:
        static void saveNvsPeers();
    public:
        void printAutoconnectPeers();
    
    private:
        static void sendEspNow(uint8_t *peer_addr, espnow_data_t *data);

    protected:
        static constexpr uint8_t broadcastMAC[ESP_NOW_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        
    public:
        void sendEspNow(espNow_peer_struct_t *peer, uint8_t *data);
        void sendEspNow(espNow_peer_struct_t *peer, uint8_t dest, uint8_t src, uint8_t type);
        void sendEspNow(espNow_peer_struct_t *peer, uint8_t dest, uint8_t src, uint8_t type, uint8_t len, uint8_t *data);
        void sendEspNow(espNow_peer_struct_t *peer, uint8_t dest, uint8_t src, uint8_t type, int8_t len, int8_t *data);
        void sendEspNow(espNow_peer_struct_t *peer, uint8_t dest, uint8_t src, uint8_t type, uint16_t *data);

};

#endif