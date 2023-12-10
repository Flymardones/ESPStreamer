#pragma once

#include "esp_a2dp_api.h"
#include "esp_timer.h"
#include "I2SOutput.hpp"

#define A2DP_48000_mask 0x10
#define A2DP_44100_mask 0x20
#define A2DP_32000_mask 0x40
#define A2DP_16000_mask 0x80

class BluetoothHandler; //Forward declare bluetoothhandler so a2dp can communicate with the handler.
class DeviceControl;

class A2DPControl {
private:
    DeviceControl* deviceController = nullptr;
    I2SOutput* i2s = nullptr;
    bool enabled = false;
    bool connected = false;
    bool abnormal_disconnect = false;
    int samplerate = 48000; //For switching between radio and BT on the fly, we need to save the SBC samplerate as its not provided again after initial connection

    void connectionEvent(esp_a2d_cb_param_t* parameter);
    void stateEvent(esp_a2d_cb_param_t* parameter);
    void configEvent(esp_a2d_cb_param_t* parameter);
public:
    void control_callback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t* parameter);
    void data_callback(const uint8_t* data, uint32_t size);
    void setI2SOutput(I2SOutput* output);
    void disableOutput();
    void enableOutput();
    bool isConnected();
    bool abnormalDisconnection();
    void setDeviceController(DeviceControl* deviceController);
};