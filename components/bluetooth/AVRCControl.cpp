#include "AVRCControl.hpp"

void AVRCControl::callback(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t* parameter) {
    switch (event) {
    case ESP_AVRC_CT_METADATA_RSP_EVT:
        metaDataEvent(parameter);
        break;
    case ESP_AVRC_CT_CONNECTION_STATE_EVT:
        connectionStateEvent(parameter);
        break;
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT:
        passthroughEvent(parameter);
        break;
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT:
        changeNotifyEvent(parameter);
        break;
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT: 
        remoteFeatureEvent(parameter);
        break;

    default:
        printf("Unknown AVRC event: %d\n", event);
        break;
    }
}


void AVRCControl::metaDataEvent(esp_avrc_ct_cb_param_t* parameter) {

}

void AVRCControl::connectionStateEvent(esp_avrc_ct_cb_param_t* parameter) {
    connected = parameter->conn_stat.connected;
}

void AVRCControl::passthroughEvent(esp_avrc_ct_cb_param_t* parameter) {
    //Disse er egentlig alle ligegyldige for at a2dp virker så vidt jeg kan se
}  

void AVRCControl::changeNotifyEvent(esp_avrc_ct_cb_param_t* parameter) {
    //This is where we tell what metadata we are capable of receiving, if any.
}

void AVRCControl::remoteFeatureEvent(esp_avrc_ct_cb_param_t* parameter) {
    //Send features
}

bool AVRCControl::play() {
    if (connected) {
        esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_PLAY, ESP_AVRC_PT_CMD_STATE_PRESSED);
        esp_avrc_ct_send_passthrough_cmd(1, ESP_AVRC_PT_CMD_PLAY, ESP_AVRC_PT_CMD_STATE_RELEASED);
        return true;
    }
    return false;
}

bool AVRCControl::pause() {
    if (connected) {
        esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_PAUSE, ESP_AVRC_PT_CMD_STATE_PRESSED);
        esp_avrc_ct_send_passthrough_cmd(1, ESP_AVRC_PT_CMD_PAUSE, ESP_AVRC_PT_CMD_STATE_RELEASED);
        return true;
    }
    return false;
}

bool AVRCControl::next() {
    if (connected) {
        esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_FORWARD, ESP_AVRC_PT_CMD_STATE_PRESSED);
        esp_avrc_ct_send_passthrough_cmd(1, ESP_AVRC_PT_CMD_FORWARD, ESP_AVRC_PT_CMD_STATE_RELEASED);
        return true;
    }
    return false;
}

bool AVRCControl::previous() {
    if (connected) {
        esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_BACKWARD, ESP_AVRC_PT_CMD_STATE_PRESSED);
        esp_avrc_ct_send_passthrough_cmd(1, ESP_AVRC_PT_CMD_BACKWARD, ESP_AVRC_PT_CMD_STATE_RELEASED);
        return true;
    }
    return false;
}

