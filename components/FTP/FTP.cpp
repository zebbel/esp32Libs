#include "FTP.h"

#include "ftpServer.h"


FTP::FTP(){
    
}

void FTP::init(){
    // Initialize NVS
	esp_err_t ret;
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	// Initialize WiFi
	ESP_LOGI("FTP", "ESP_WIFI_MODE_STA");
	ESP_ERROR_CHECK(init_wifi());

	// Initialize mDNS
	initialise_mdns();

	// Obtain local IP address
	esp_netif_ip_info_t ip_info;
	ESP_ERROR_CHECK(esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info));

	// Print the local IP address
	ESP_LOGI("FTP", "IP Address : " IPSTR, IP2STR(&ip_info.ip));
	ESP_LOGI("FTP", "Subnet mask: " IPSTR, IP2STR(&ip_info.netmask));
	ESP_LOGI("FTP", "Gateway	  : " IPSTR, IP2STR(&ip_info.gw));

    // Create FTP server task
	xTaskCreate(ftp_task, "FTP", 1024*6, NULL, 2, &ftpTask);
}

void FTP::deinit(){
    vTaskSuspend(ftpTask);
}

esp_err_t FTP::init_wifi(){
    s_wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_t *netif = esp_netif_create_default_wifi_sta();
	assert(netif);

#if CONFIG_STATIC_IP
	ESP_LOGI("FTP", "CONFIG_STATIC_IP_ADDRESS=[%s]",CONFIG_STATIC_IP_ADDRESS);
	ESP_LOGI("FTP", "CONFIG_STATIC_GW_ADDRESS=[%s]",CONFIG_STATIC_GW_ADDRESS);
	ESP_LOGI("FTP", "CONFIG_STATIC_NM_ADDRESS=[%s]",CONFIG_STATIC_NM_ADDRESS);

	/* Stop DHCP client */
	ESP_ERROR_CHECK(esp_netif_dhcpc_stop(netif));
	ESP_LOGI("FTP", "Stop DHCP Services");

	/* Set STATIC IP Address */
	esp_netif_ip_info_t ip_info;
	memset(&ip_info, 0 , sizeof(esp_netif_ip_info_t));
	ip_info.ip.addr = ipaddr_addr(CONFIG_STATIC_IP_ADDRESS);
	ip_info.netmask.addr = ipaddr_addr(CONFIG_STATIC_NM_ADDRESS);
	ip_info.gw.addr = ipaddr_addr(CONFIG_STATIC_GW_ADDRESS);
	ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));

	/* Set DNS Server */
	ESP_ERROR_CHECK(set_dns_server(netif, ipaddr_addr("8.8.8.8"), ESP_NETIF_DNS_MAIN));
	ESP_ERROR_CHECK(set_dns_server(netif, ipaddr_addr("8.8.4.4"), ESP_NETIF_DNS_BACKUP));
#endif // CONFIG_STATIC_IP

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, this, &instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, this, &instance_got_ip));


	wifi_config_t wifi_config = {};
	strncpy((char*)wifi_config.sta.ssid, CONFIG_ESP_WIFI_ST_SSID, sizeof(wifi_config.sta.ssid));
	strncpy((char*)wifi_config.sta.password, CONFIG_ESP_WIFI_ST_PASSWORD, sizeof(wifi_config.sta.password));
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	wifi_config.sta.pmf_cfg = {.capable = true, .required = false};

	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	/* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
	 * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
	EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

	/* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually happened. */
	esp_err_t ret_value = ESP_OK;
	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI("FTP", "connected to ap SSID:%s", CONFIG_ESP_WIFI_ST_SSID);
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGE("FTP", "Failed to connect to SSID:%s", CONFIG_ESP_WIFI_ST_SSID);
		ret_value = ESP_FAIL;
	} else {
		ESP_LOGE("FTP", "UNEXPECTED EVENT");
		ret_value = ESP_ERR_INVALID_STATE;
	}

	/* The event will not be processed after unregister */
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
	vEventGroupDelete(s_wifi_event_group); 
	ESP_LOGI("FTP", "init_wifi finished.");
	return ret_value; 
}

void FTP::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    FTP* ftp = reinterpret_cast<FTP*>(arg); //obtain the instance pointer

	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (ftp->s_retry_num < CONFIG_ESP_MAXIMUM_RETRY) {
			esp_wifi_connect();
			xEventGroupClearBits(ftp->s_wifi_event_group, WIFI_CONNECTED_BIT);
			ftp->s_retry_num++;
			ESP_LOGI("FTP", "retry to connect to the AP");
		} else {
			xEventGroupSetBits(ftp->s_wifi_event_group, WIFI_FAIL_BIT);
			ftp->wifi_connected = false;
		}
		ESP_LOGI("FTP","connect to the AP fail");
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
		ESP_LOGI("FTP", "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		ftp->s_retry_num = 0;
		xEventGroupSetBits(ftp->s_wifi_event_group, WIFI_CONNECTED_BIT);
		ftp->ip = event->ip_info.ip;
		ftp->wifi_connected = true;
	}
}

esp_err_t FTP::set_dns_server(esp_netif_t *netif, uint32_t addr, esp_netif_dns_type_t type){
	if (addr && (addr != IPADDR_NONE)) {
		esp_netif_dns_info_t dns;
		dns.ip.u_addr.ip4.addr = addr;
		dns.ip.type = IPADDR_TYPE_V4;
		ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, type, &dns));
	}
	return ESP_OK;
}

void FTP::initialise_mdns(){
	//initialize mDNS
	ESP_ERROR_CHECK( mdns_init() );
	//set mDNS hostname (required if you want to advertise services)
	ESP_ERROR_CHECK( mdns_hostname_set(CONFIG_MDNS_HOSTNAME) );
	ESP_LOGI("FTP", "mdns hostname set to: [%s]", CONFIG_MDNS_HOSTNAME);
}