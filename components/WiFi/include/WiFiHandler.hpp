#ifndef ESPSTREAMER_WIFIHANDLER_H
#define ESPSTREAMER_WIFIHANDLER_H

#include <string>
#include "esp_wifi.h"

struct WiFiInfo {
    std::string ssid;
    std::string passkey;
    int signalStrength = 0;
    wifi_auth_mode_t authenticationMode = WIFI_AUTH_OPEN;
};

class WiFiHandler {
public:
    WiFiHandler();
    ~WiFiHandler();

    int scan(WiFiInfo* output, uint16_t maxNetworks);
    bool connect(WiFiInfo* info);
    bool disconnect();
    //std::string getIPAsString();
    //bool isConnected();

private:
    esp_netif_t* netif;
    void copyStringToBytes(std::string& string, uint8_t* buffer, uint32_t maxSize);


};

#endif //ESPSTREAMER_WIFIHANDLER_H
