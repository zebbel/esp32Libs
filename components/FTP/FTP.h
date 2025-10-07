#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <esp_log.h>
#include "nvs_flash.h"
#include "esp_mac.h" // for MACSTR
#include "esp_wifi.h"
#include "mdns.h"
#include "lwip/dns.h"

class FTP{
    private:
        /* FreeRTOS event group to signal when we are connected*/
        EventGroupHandle_t s_wifi_event_group;

        /* The event group allows multiple bits for each event, but we only care about two events:
        * - we are connected to the AP with an IP
        * - we failed to connect after the maximum amount of retries */
        #define WIFI_CONNECTED_BIT BIT0
        #define WIFI_FAIL_BIT BIT1

        uint8_t s_retry_num = 0;

        TaskHandle_t ftpTask;

        esp_err_t init_wifi();

        static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
        static esp_err_t set_dns_server(esp_netif_t *netif, uint32_t addr, esp_netif_dns_type_t type);
        void initialise_mdns();

    public:
        bool wifi_connected = false;
        esp_ip4_addr_t ip;
        FTP();
        void init();
        void deinit();
};