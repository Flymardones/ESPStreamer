#include "GAPControl.hpp"
#include "DeviceControl.hpp"

#define SSP 1
#define PINCODE 0

void GAPControl::callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* parameter) {
    switch(event) {
        case ESP_BT_GAP_AUTH_CMPL_EVT: // Authentication Complete
            authentication_complete(parameter);
            break;
        #if PINCODE
        case ESP_BT_GAP_PIN_REQ_EVT: // Legacy Pairing Pin Request
            #if DEBUG
            printf("Legacy Pairing Pin Request\n");
            #endif
            handle_pin_request(parameter);
            break;
        #endif

        #if SSP    
        case ESP_BT_GAP_CFM_REQ_EVT: // Security Simple Pairing User Confirmation request
            #if DEBUG
            printf("SSP Confirm reply!\n");
            #endif
            esp_bt_gap_ssp_confirm_reply(parameter->cfm_req.bda, true);
            break;
        case ESP_BT_GAP_KEY_NOTIF_EVT: // Security Simple Pairing Passkey Notification
            //ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%"PRIu32, parameter->key_notif.passkey);
            #if DEBUG
            printf("ESP_BT_GAP_KEY_NOTIF_EVT passkey: %d\n", parameter->key_notif.passkey);
            #endif
            break;
        case ESP_BT_GAP_KEY_REQ_EVT: // Security Simple Pairing Passkey request
            //ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
            #if DEBUG
            printf("Please enter the passkey!\n");
            #endif
            //esp_bt_gap_ssp_passkey_reply(parameter->key_req.bda, true, 123456);
            break;
        case ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT:
            #if DEBUG
            printf("ACL connection complete!\n");    
            #endif
            break;
        #endif
        default:
            #if DEBUG
            printf("Unknown GAP callback received: %d\n", event);
            #endif
            break;
    }
}

void GAPControl::authentication_complete(esp_bt_gap_cb_param_t* parameter) {
    if (parameter->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
        #if DEBUG
        printf("GAP Authentication successful!\n");
        #endif
        if (deviceController != nullptr && !deviceController->btHandler->a2dpController.isConnected()) {
            deviceController->setBluetoothOwner(parameter->auth_cmpl.bda);
            #if DEBUG
            printf("Setting BT owner in gap callback: ");
            for(int i = 0; i < sizeof(parameter->auth_cmpl.bda); i++) {
                printf("%02X ", parameter->auth_cmpl.bda[i]);
            }
            printf("\n");
            #endif
        }
    } else {
        #if DEBUG
        printf("GAP Authentication failed. Status: %d\n", parameter->auth_cmpl.stat);
        #endif
    }
}

void GAPControl::handle_pin_request(esp_bt_gap_cb_param_t* parameter) {
    esp_bt_pin_code_t pin;
    uint8_t pinLength = 4;
    #if DEBUG
    printf("In handle pin request\n");   
    #endif 
    if (parameter->pin_req.min_16_digit) {    
        pinLength =  16;
        memset(pin, '0', ESP_BT_PIN_CODE_LEN);
    } else {
        pin[0] = '1';
        pin[1] = '2';
        pin[2] = '3';
        pin[3] = '4';
    }
    #if DEBUG
    printf("Expecting this pin code: %.*s\n", pinLength, pin);
    #endif
    esp_bt_gap_pin_reply(parameter->pin_req.bda, true, pinLength, pin);
}

void GAPControl::setDeviceController(DeviceControl* deviceController) {
    this->deviceController = deviceController;
}