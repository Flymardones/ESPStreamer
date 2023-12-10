#include "DeviceSetup.hpp"


DeviceSetup::DeviceSetup() {
    printf("Device Setup Constructor\n");

    if (setup_gpio() != ESP_OK) {
        setup = false;
    }

    if (setup_i2c() != ESP_OK) {
        setup = false;
    }

    if (setup_nfc() != ESP_OK) {
        setup = false;
    }

    if (setup_led() != ESP_OK) {
        setup = false;
    }

    if (setup_security_param() != ESP_OK) {
        setup = false;
    }
}


DeviceSetup::~DeviceSetup() {

}


esp_err_t DeviceSetup::setup_gpio() {
    // NFC-tag interrupt on rf signal
    gpio_config_t* nfcConfig = new gpio_config_t;
    nfcConfig->intr_type = GPIO_INTR_POSEDGE;
    nfcConfig->mode = GPIO_MODE_INPUT;
    nfcConfig->pin_bit_mask = 1ULL<<GPIO_NUM_5;
    nfcConfig->pull_down_en = GPIO_PULLDOWN_DISABLE;
    nfcConfig->pull_up_en = GPIO_PULLUP_DISABLE;

    if(gpio_config(nfcConfig) != ESP_OK) {
        printf("Failed to config NFC-tag!\n");
        return ESP_FAIL;
    }
    delete nfcConfig;

    // Button for pairing mode
    gpio_config_t* buttonConfig = new gpio_config_t;
    buttonConfig->intr_type = GPIO_INTR_POSEDGE;
    buttonConfig->mode = GPIO_MODE_INPUT;
    buttonConfig->pin_bit_mask = 1ULL<<GPIO_NUM_36;
    buttonConfig->pull_down_en = GPIO_PULLDOWN_DISABLE;
    buttonConfig->pull_up_en = GPIO_PULLUP_DISABLE;
    
    if(gpio_config(buttonConfig) != ESP_OK){
        printf("Failed to config button!\n");
        return ESP_FAIL;
    }
    delete buttonConfig;

    return ESP_OK;
}

esp_err_t DeviceSetup::setup_i2c() {
    // Configure I2C controller 0 for master
    i2c_config_t conf0 = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {.clk_speed = 400000},
    };
    
    // Configure I2C bus with given configuration
    if (i2c_param_config(I2C_NUM_0, &conf0) != ESP_OK) {
        printf("I2C config failed\n");
        return ESP_FAIL;
    }
    
    // Install I2C driver
    if (i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) != ESP_OK ) {
        printf("I2C driver install failed\n");
        return ESP_FAIL;
    }

    return ESP_OK;
}

Ndef_Bluetooth_OOB_t DeviceSetup::setup_bluetooth_oob() {
    // Get esp32 bluetooth mac address for bt ndef message
    const uint8_t *bt_address = esp_bt_dev_get_address();
    
    if(bt_address == NULL) {
        printf("Failed to get BT Address!\n");
    }
    else {
        #if DEBUG
        printf("Got the following BT Address: ");
        for(int i = 0; i < 6; i++) {
            printf("%02X ", bt_address[i]);
        }
        printf("\n");
        #endif
    }


    // Configure Bluetooth OOB data to send
    Ndef_Bluetooth_OOB_t bt_oob = {
        .Type = NDEF_BLUETOOTH_BREDR,
        .DeviceAddress = {bt_address[0], bt_address[1], bt_address[2], bt_address[3], bt_address[4], bt_address[5]},
        .OptionalMask = NDEF_BLUETOOTH_OPTION(BLUETOOTH_EIR_DEVICE_CLASS) |
                        NDEF_BLUETOOTH_OPTION(BLUETOOTH_EIR_SERVICE_CLASS_UUID_COMPLETE_16),
        .ClassUUID16 = {0x111E, 0x110B},
        .nbUUID16 = 2,
        .DeviceClass = {0x0A, 0x04, 0x20}
    };

    return bt_oob;
}  

esp_err_t DeviceSetup::setup_nfc() {
    ST25DV st25dv;

    Ndef_Bluetooth_OOB_t bt_oob = setup_bluetooth_oob();

    // Init ST25DV module
    if(st25dv.begin() != ESP_OK) { 
        printf("NFC Tag Init Failed!\n");
        return ESP_FAIL;
    }

    // Write Bluetooth BR/EDR OOB NDEF message
    if (st25dv.writeBluetoothOOB(&bt_oob, NULL) != ESP_OK){
        printf("Failed to append BT OOB!\n");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DeviceSetup::setup_led() {
    // LED for displaying pairing mode and connection state
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);
    ledc_channel.gpio_num = GPIO_NUM_33;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.channel = LEDC_CHANNEL_0;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel.duty = 0;
    ledc_channel.hpoint = 0;
    ledc_channel_config(&ledc_channel);

    // Install ledc fade function
    if (ledc_fade_func_install(0) != ESP_OK) {
        printf("Failed to install ledc fade\n");
        return ESP_FAIL;
    }

    return ESP_OK;
}   

esp_err_t DeviceSetup::setup_security_param() {

    #if PIN_CODE
    // Set default parameters for Secure Simple Pairing 
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
    // invokes callbacks
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);
    #else
    // Set default parameters for Secure Simple Pairing 
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_NONE;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
    // no callbacks
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);
    #endif

    return ESP_OK;
}   

 
