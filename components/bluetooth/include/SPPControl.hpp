#pragma once

#include "esp_spp_api.h"
#include "QPrint.hpp"

class SPPControl : public QPrinter {
    private:
        bool spp_server_running = false;
        uint32_t serverHandle = 0;

        void initialize(esp_spp_cb_param_t* parameter);
        void uninitialize(esp_spp_cb_param_t* parameter);

    public:
        void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t* parameter);
        void print(std::string string);
        esp_err_t disconnect();
        

};