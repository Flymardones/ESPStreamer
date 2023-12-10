#include "SPPControl.hpp"

void SPPControl::callback(esp_spp_cb_event_t event, esp_spp_cb_param_t* parameter) {
    switch(event) {
        case ESP_SPP_INIT_EVT:
            initialize(parameter);
            break;
        case ESP_SPP_UNINIT_EVT:
            uninitialize(parameter);
            break;
        case ESP_SPP_CLOSE_EVT:
            QPrint::println("SPP Closed");
            serverHandle = parameter->close.handle;
            disconnect();
            break;
        case ESP_SPP_START_EVT:
            QPrint::println("SPP Started");
            serverHandle = parameter->start.handle;
            break;
        case ESP_SPP_DATA_IND_EVT:
            //This should be saved in buffer and not printed as print is slow and this is a high priority thread
            QPrint::print(std::string((char*)parameter->data_ind.data, parameter->data_ind.len), controlInput);
            break;
        default:
            QPrint::println("Unhandled SPP event: " + std::to_string(event));
            break;
    }
}

void SPPControl::initialize(esp_spp_cb_param_t* parameter) {
    if (parameter->init.status == ESP_SPP_SUCCESS) {
        if(esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_SLAVE, 0, "SPP QPrinter") != ESP_OK) {
            printf("Failed to start spp server!\n");
        }
        spp_server_running = true;
        QPrint::registerPrinter(this, controlOutput);
    } else {
        QPrint::println("SPP Init event with non-success status: " + std::to_string(parameter->init.status));
    }
}


void SPPControl::uninitialize(esp_spp_cb_param_t* parameter) {
    if (spp_server_running) {

        esp_spp_stop_srv();
    }
}

void SPPControl::print(std::string string) {
    esp_spp_write(serverHandle, string.size(), (uint8_t*) string.c_str());
}

esp_err_t SPPControl::disconnect() {
    static uint8_t ret = 0;
    
    ret = esp_spp_disconnect(serverHandle);
    if(ret != ESP_OK) {
        printf("ESP Error Code: %d\n", ret);
        return ret;
    }
    
    return ret;
}
