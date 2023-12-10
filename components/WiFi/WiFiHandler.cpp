#include "WiFiHandler.hpp"
#include "QPrint.hpp"
#include "ICESleep.hpp"
#include <cstring>


//Just an example for later...
void wifi_event(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        QPrint::print("got ip: " + std::to_string(event->ip_info.ip.addr) + "\n");
    }
}


WiFiHandler::WiFiHandler() {
    esp_netif_init();
    esp_event_loop_create_default();

    netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifiConfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifiConfig);

    /*
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event, NULL, &instance_got_ip)); 
    */

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
}

WiFiHandler::~WiFiHandler() {
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_netif_destroy_default_wifi(netif);
}

/*
 * This is blocking. Be sure to use this in a task that can handle the wait time of a scan.
 */
int WiFiHandler::scan(WiFiInfo* output, uint16_t maxNetworks) {
    if (maxNetworks > 64) { //Set a limit to how much memory we will use..
        return -1;
    }

    esp_wifi_scan_start(NULL, true);
    wifi_ap_record_t* foundSSIDs = new wifi_ap_record_t[maxNetworks];
    esp_wifi_scan_get_ap_records(&maxNetworks, foundSSIDs);

    uint16_t foundNetworks = 0;
    esp_wifi_scan_get_ap_num(&foundNetworks);

    QPrint::println("Found networks: " + std::to_string(foundNetworks));

    int networksToCopy = foundNetworks;
    if (foundNetworks > maxNetworks) { //Only copy as many networks as we are allowed
        networksToCopy = maxNetworks;
    }

    for (int i = 0; i < networksToCopy; i++) {
        output[i].ssid = (char*) foundSSIDs[i].ssid;
        output[i].signalStrength = foundSSIDs[i].rssi;
        output[i].authenticationMode = foundSSIDs[i].authmode;
    }

    delete[] foundSSIDs;
    return networksToCopy;
}

bool WiFiHandler::connect(WiFiInfo *info) {
    esp_wifi_stop();

    wifi_config_t wifiConfig = {};
    copyStringToBytes(info->ssid, wifiConfig.sta.ssid, 32);
    copyStringToBytes(info->passkey, wifiConfig.sta.password, 64);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifiConfig);
    esp_wifi_start();

    ICESleep(1000);

    esp_wifi_connect();

    return true;
}

void WiFiHandler::copyStringToBytes(std::string& string, uint8_t* buffer, uint32_t maxSize) {
    if (string.length() > maxSize) {
        memcpy(buffer, string.c_str(), maxSize);
    } else {
        memcpy(buffer, string.c_str(), string.size());
    }
}
/*
std::string WiFiHandler::getIPAsString() {
    tcpip_adapter_ip_info_t ipInfo; 
    char ipString[16];
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
    sprintf(ipString, "%d.%d.%d.%d",
        (ipInfo.ip.addr & 0x000000ff),
        (ipInfo.ip.addr & 0x0000ff00) >> 8,
        (ipInfo.ip.addr & 0x00ff0000) >> 16,
        (ipInfo.ip.addr & 0xff000000) >> 24);

    return std::string(ipString);
}

bool WiFiHandler::isConnected() {
    tcpip_adapter_ip_info_t ipInfo;
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);

    if (ipInfo.ip.addr != 0) {
        return true;
    }

    return false;
}
*/