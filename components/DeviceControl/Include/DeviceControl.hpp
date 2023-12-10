//
// Created by Kris on 09/05/2022.
//

#ifndef ESPSTREAMER_DEVICECONTROL_HPP
#define ESPSTREAMER_DEVICECONTROL_HPP

#include "NVS.hpp"
//#include "IcecastHTTPClient.hpp"
#include "BluetoothHandler.hpp"
#include "UARTHandler.hpp"
#include "DeviceSetup.hpp"
//#include "WiFiHandler.hpp"
#include "QPrint.hpp"
#include "esp_bt_defs.h"
#include "LEDDriver.hpp"
#include "ICETimer.hpp"
#include "ICETask.hpp"
#include "ICESleep.hpp"

#define maxWifisToScan 32

#define DEBUG 0

#define GPIO_BUTTON_PIN GPIO_NUM_36
#define BUTTON_PRESS_TIME 20
static bool tflag;

class TimerTaskCallback : virtual public ICETask{
public:
    TimerTaskCallback() {};
    void runTask(void* unused){
        if (tflag){
            tflag = false;
        }
        else
        {
            tflag = true;
        }
    }; //argument should be pointer to flag
};

class DeviceControl : QPrinter {
private:
    enum DeviceControlState {
        DEVICE_IDLE,
        DEVICE_CHOOSING_WIFI,
        DEVICE_ENTER_WIFI_PASS,
    };

    DeviceControlState deviceState = DEVICE_IDLE;
    int foundWiFis = 0;
    int chosenWiFi;

    void printWifis();

    enum DeviceBluetoothState{
        PAIRING_MODE,
        CONNECTED,
        NOT_CONNECTED_NOT_PAIRING,
        NO_DSP_NO_MIXAMP,
        BUTTON_PRESS,
        BUTTON_PRESS_DONE
    };
    DeviceBluetoothState deviceBluetoothState = NO_DSP_NO_MIXAMP;
    DeviceBluetoothState previousdeviceBluetoothState = PAIRING_MODE;


    uint8_t LEDC_timer_bitmask = 0;
    bool tflag_old = false;

    uint8_t buttoncnt = 0;
    void buttonIndicatorLEDTimer();
    bool disconnectFlag = false;



public:
    DeviceControl();
    NVS nvs;
    I2SOutput i2sOutput;
    BluetoothHandler* btHandler = nullptr;
    UARTHandler uartHandler;
    LEDDriver ledc_controller;
    DeviceSetup* deviceSetup = nullptr;
    bool nfc_tag_scanned = false;
    bool pairing_mode = false;
    bool connected_after_nfc = false;
    bool timer_started = false;
    ledc_channel_config_t ledc_channel = {};
    esp_timer_handle_t timer;
    void startBluetooth();
    void setBluetoothOwner(esp_bd_addr_t deviceAddress);
    void removeBluetoothOwner();
    void print(std::string input) override; //This takes a string input and processes it depending on the state of DeviceControl
    void processIdleInput(std::string input);
    void zeroOwner();
    void newOwner(esp_bd_addr_t new_owner);
    bool preventAutoreconnect(esp_bd_addr_t new_owner);
    void update_LEDC_variables(bool flag);
    uint8_t get_device_bluetooth_state() {deviceBluetoothState;}
    void set_bluetooth_state_flag(uint8_t state);
    void update_bluetooth_state_LEDC_timer();
    void buttonCheck();
    void start_tasks();

    uint8_t timer_cnt = 0; //move over to private once finished

};

#endif //ESPSTREAMER_DEVICECONTROL_HPP
