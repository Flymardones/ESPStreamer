#include "ST25DVSensor.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "esp_gap_bt_api.h"  
#include "esp_bt_device.h"
#include "esp_err.h"


#define PIN_CODE 0


class DeviceSetup {
    private:
    esp_err_t setup_gpio();
    esp_err_t setup_i2c();
    esp_err_t setup_nfc();
    esp_err_t setup_led();
    esp_err_t setup_security_param();
    Ndef_Bluetooth_OOB_t setup_bluetooth_oob();

    public:
    DeviceSetup();
    ~DeviceSetup();
    bool setup = true;
    ledc_channel_config_t ledc_channel = {};
};