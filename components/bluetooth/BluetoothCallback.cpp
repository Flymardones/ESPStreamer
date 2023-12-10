#include "BluetoothCallback.hpp"

GAPControl* BluetoothCallbackManager::gapController = nullptr;
SPPControl* BluetoothCallbackManager::sppController = nullptr;
A2DPControl* BluetoothCallbackManager::a2dpController = nullptr;
AVRCControl* BluetoothCallbackManager::avrcController = nullptr;

int BluetoothCallbackManager::registerCallbacks() {
    if (esp_bt_gap_register_callback(BluetoothCallbackManager::GAPCallback) != ESP_OK) {
        return -1;
    }

    if (esp_spp_register_callback(BluetoothCallbackManager::SPPCallback) != ESP_OK) {
        return -2;
    }

    if (esp_a2d_register_callback(BluetoothCallbackManager::A2DPControlCallback) != ESP_OK) {
        return -3;
    }

    if (esp_a2d_sink_register_data_callback(BluetoothCallbackManager::A2DPDataCallback) != ESP_OK) {
        return -4;
    }

    if (esp_avrc_ct_register_callback(BluetoothCallbackManager::AVRCCallback) != ESP_OK) {
        return -5;
    }

    return 0;
}

void BluetoothCallbackManager::setGAPController(GAPControl* controller) {
    BluetoothCallbackManager::gapController = controller;
}
void BluetoothCallbackManager::setSPPController(SPPControl* controller) {
    BluetoothCallbackManager::sppController = controller;
}
void BluetoothCallbackManager::setA2DPController(A2DPControl* controller) {
    BluetoothCallbackManager::a2dpController = controller;
}
void BluetoothCallbackManager::setAVRCController(AVRCControl* controller) {
    BluetoothCallbackManager::avrcController = controller;
}


void BluetoothCallbackManager::GAPCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* parameter) {
    BluetoothCallbackManager::gapController->callback(event, parameter);
}

void BluetoothCallbackManager::SPPCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t* parameter) {
    BluetoothCallbackManager::sppController->callback(event, parameter);
}

void BluetoothCallbackManager::A2DPControlCallback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t* parameter) {
    BluetoothCallbackManager::a2dpController->control_callback(event, parameter);
}

void BluetoothCallbackManager::A2DPDataCallback(const uint8_t *data, uint32_t size) {
    BluetoothCallbackManager::a2dpController->data_callback(data, size);
}

void BluetoothCallbackManager::AVRCCallback(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t* parameter) {
    BluetoothCallbackManager::avrcController->callback(event, parameter);
}