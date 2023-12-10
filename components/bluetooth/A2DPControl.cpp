#include "A2DPControl.hpp"
#include "BluetoothHandler.hpp"
#include "DeviceControl.hpp"

void A2DPControl::control_callback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t* parameter) {
    switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
        connectionEvent(parameter);
        break;
    case ESP_A2D_AUDIO_STATE_EVT:
        stateEvent(parameter);
        break;
    case ESP_A2D_AUDIO_CFG_EVT:
        configEvent(parameter);
        break;
    case ESP_A2D_PROF_STATE_EVT:
        #if DEBUG
        printf("A2DP Init Completed!\n");
        #endif
        break;
        
    default:
        #if DEBUG
        printf("Received unknown A2DP event: %d\n", event);
        #endif
        break;
    }
}

void A2DPControl::data_callback(const uint8_t* data, uint32_t size) {
    if (!enabled) {
        return;   //If we are not enabled and someone tries to play music, we just do nothing..
    }

    if (i2s != nullptr) {
        i2s->writeDirect(data, size);
    }
}

void A2DPControl::setI2SOutput(I2SOutput* output) {
    if (output != nullptr) {
        this->i2s = output;
    }
}

void A2DPControl::connectionEvent(esp_a2d_cb_param_t* parameter) {
    if (parameter->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) { // Disconnected
        if(deviceController != nullptr) {
            if(parameter->conn_stat.disc_rsn == ESP_A2D_DISC_RSN_ABNORMAL) { // If signal is lost (out of range)
                #if DEBUG
                printf("Abnormal disconnect detected!\n");
                #endif
                abnormal_disconnect = true;
            }
            else {
                if(!abnormal_disconnect) { // If no abnormal disconnection or the nfc tag has been scanned
                    deviceController->zeroOwner();
                    #if DEBUG
                    QPrint::println("A2DP was disconnected");
                    #endif
                    if(deviceController->pairing_mode || deviceController->nfc_tag_scanned) {
                        deviceController->btHandler->setDiscoverable(true);
                    }
                }
            }
            uart_write_bytes(UART_NUM_0, "$DISCONNECTED\n", strlen("$DISCONNECTED\n"));
            connected = false;
            enabled = false;
        }
    } 
    else if (parameter->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) { // Connected
        if(deviceController != nullptr) {

            if(abnormal_disconnect) { // If a device has managed to connect after an abnormal disconnect 
                #if DEBUG
                printf("Device has reconnected!\n");
                #endif
                abnormal_disconnect = false;
            }  
            // On boot device should try and reconnect to last connected device and then set this value to false
            if(deviceController->btHandler->first_time_boot) { 
                deviceController->btHandler->first_time_boot = false;
                #if DEBUG
                printf("Last BT Owner has reconnected!\n");
                #endif
            }
            
            if (!deviceController->preventAutoreconnect(parameter->conn_stat.remote_bda)) { // If new device connected, else prevent last connected device from reconnecting
                deviceController->newOwner(parameter->conn_stat.remote_bda);
                #if DEBUG
                QPrint::println("A2DP was connected");
                #endif

                if(deviceController->nfc_tag_scanned == true) { // if a device has connected after nfc tag enable intr again
                    deviceController->nfc_tag_scanned = false;
                    deviceController->connected_after_nfc = true;
                }
                else if(deviceController->pairing_mode) { // If remote device connects in pairing mode set it to false
                    deviceController->pairing_mode = false;
                }
                uart_write_bytes(UART_NUM_0, "$CONNECTED\n", strlen("$CONNECTED\n"));
                connected = true;
            }
        } 
    }
}

void A2DPControl::stateEvent(esp_a2d_cb_param_t* parameter) {
    if (parameter->audio_stat.state == ESP_A2D_AUDIO_STATE_STARTED) {
        //Maybe we want to do something when the audio starts/stops in the future..
    }
}

void A2DPControl::configEvent(esp_a2d_cb_param_t* parameter) {
    if (parameter->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
        uint8_t esp_sbc_header = parameter->audio_cfg.mcc.cie.sbc[0];
        #if DEBUG
        printf("Setting Sample Rate\n");
        #endif
        if (i2s != nullptr) {
            if (esp_sbc_header & A2DP_48000_mask) {
                i2s->setSampleRate(48000);
                samplerate = 48000;
            } else if (esp_sbc_header & A2DP_44100_mask) {
                i2s->setSampleRate(44100);
                samplerate = 44100;
            } else if (esp_sbc_header & A2DP_32000_mask) {
                i2s->setSampleRate(32000);
                samplerate = 32000;
            } else {
                i2s->setSampleRate(16000);
                samplerate = 16000;
            }
            #if DEBUG
            printf("Sample Rate has been set to %d\n", samplerate);
            #endif
            enabled = true;
        }
    }
}

void A2DPControl::disableOutput() {
    enabled = false;
}

void A2DPControl::enableOutput() {
    if (i2s != nullptr) {
        i2s->setSampleRate(samplerate);
        enabled = true;        
    }
}

bool A2DPControl::isConnected() {
    return connected;
}
bool A2DPControl::abnormalDisconnection() {
    return abnormal_disconnect;
}

void A2DPControl::setDeviceController(DeviceControl* deviceController) {
    if (deviceController != nullptr) {
        this->deviceController = deviceController;
    }
}
