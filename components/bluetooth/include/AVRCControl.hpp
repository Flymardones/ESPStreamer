#pragma once

#include "esp_avrc_api.h"

class AVRCControl {
 private:
    bool connected = false;
    void metaDataEvent(esp_avrc_ct_cb_param_t* parameter);
    void connectionStateEvent(esp_avrc_ct_cb_param_t* parameter);
    void passthroughEvent(esp_avrc_ct_cb_param_t* parameter);
    void changeNotifyEvent(esp_avrc_ct_cb_param_t* parameter);
    void remoteFeatureEvent(esp_avrc_ct_cb_param_t* parameter);

public:
    void callback(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t* parameter);
    bool play();
    bool pause();
    bool next();
    bool previous();
};