#pragma once

#include <stdio.h>
#include <esp_log.h>
#include <string.h>
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "nvs_flash.h"


#include "CRSF_CRC.h"

#include "../include/typedefDeviceAddress.h"
#include "../include/typedefsBroadcast.h"
#include "../include/typedefsExtended.h"


#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

/* ESPNOW can work in both station and softap mode. It is configured in menuconfig. */
#if CONFIG_ESPNOW_WIFI_MODE_STATION
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   WIFI_IF_STA
#else
#define ESPNOW_WIFI_MODE WIFI_MODE_AP
#define ESPNOW_WIFI_IF   WIFI_IF_AP
#endif

#define ESPNOW_QUEUE_SIZE           6
#define ESPNOW_MAXDELAY             512

enum{
    ESPNOW_STATUS_DISCONNECTED,
    ESPNOW_STATUS_CONNECTING,
    ESPNOW_STATUS_CONNECTED
};

extern "C"{
    /**
     * @brief enum with espNow role
     */
    typedef enum{
        CRSF_ESPNOW_SLAVE,
        CRSF_ESPNOW_MASTER
    } crsf_espNow_role_t;

    typedef struct{
        uint8_t frame[200];
        int8_t rssi;
    } espNowframe;

    typedef struct{
        espNowframe frame;
        uint8_t deviceMAC[ESP_NOW_ETH_ALEN];
    } espNowRecvframe;


    class CRSF_ESPNOW: virtual public CRSF_CRC{
        private:
            crsf_espNow_role_t role;
            static QueueHandle_t espnow_queue;
            uint64_t watchdogTime;
            uint8_t broadcastMAC[ESP_NOW_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
            uint8_t deviceMAC[ESP_NOW_ETH_ALEN];
            TaskHandle_t esp_now_task;
            void (*espNowDirectFunction)(uint8_t* data);
            crsf_broadcast_frame_t watchdogFrame;

            void initWifi();
            void initEspNow();
            static void connectTask(void *pvParameter);
            static void sendCallback(const wifi_tx_info_t *sendInfo, esp_now_send_status_t status);
            static void recvCallback(const esp_now_recv_info_t *recvInfo, const uint8_t *data, int len);
            static void task(void *pvParameter);
            void handleConnectingframe(espNowRecvframe *recvframe);
            void sendWatchdog();
            void checkWatchdog();

        public:
            CRSF_ESPNOW();
            void init(crsf_espNow_role_t espNowRole);
            void deInit();
            void setEspNowDirectFunction(void (*function)(uint8_t*));
            void connectDevice();
            void send(uint8_t *data, size_t len);

            QueueHandle_t queue;
            uint8_t espNowStatus;
            int8_t rssi;
    };
}