//
// Created by Kris on 09/05/2022.
//

#include "DeviceControl.hpp"
#include "ICESleep.hpp"
#include "PrintfQPrinter.hpp"
#include "ICEString.hpp"

static QueueHandle_t interruptQueue = NULL;
DeviceControl* deviceControl;


// ISR
static void IRAM_ATTR gpio_interrupt_handler(void* args) {
    uint32_t pin_number = (uint32_t) args;
    xQueueSendFromISR(interruptQueue, &pin_number, NULL);
}


DeviceControl::DeviceControl() {
    deviceControl = this;

    printf("Device Control Constructor\n");
    //Start printfprinter for QPrint
    PrintfQPrinter* printfQPrinter = new PrintfQPrinter(); //Make QPrint debug appear in as printf messages
    QPrint::registerPrinter(printfQPrinter);
    QPrint::registerPrinter(this, controlInput); //Register the devicecontrol as a QPrint control input

    //Initialize bluetooth as bluetooth should be always on
    startBluetooth();

    deviceSetup = new DeviceSetup();

    if (deviceSetup->setup) {
        ledc_channel = deviceSetup->ledc_channel;
        start_tasks();

    }
}

void DeviceControl::startBluetooth() {
    printf("Starting Bluetooth\n");
    btHandler = new BluetoothHandler();
    btHandler->setDeviceController(this);
    btHandler->setI2SOutput(&i2sOutput);
    std::string btName = nvs.getStringValue("BTDeviceName");
    if (btName == "") {
        btName = "ESP32";
        btHandler->setDeviceName(btName);
    }
    else {
        btHandler->setDeviceName(btName);
    }

    esp_bd_addr_t owner;
    uint32_t read = nvs.getBinaryValue("BluetoothOwner", owner, ESP_BD_ADDR_LEN); // Get BT Owner from nvs partition
    btHandler->setOwner(owner);

    // If there is a owner upon boot
    if(owner[0] != 0 || owner[1] != 0 || owner[2] != 0 || owner[3] != 0 || owner[4] != 0 || owner[5] != 0) { 
        #if DEBUG    
        printf("There is a Owner\n");
        #endif
        btHandler->setDiscoverable(true);
    }
    else { // No owner
        #if DEBUG
        printf("There is no Owner\n");
        #endif
        btHandler->setDiscoverable(false);
        btHandler->first_time_boot = false; // Do not attempt to connect to an owner as there is none
    }
}

// Set new owner via nfc tag or button
static void set_new_owner(void* params){
    uint32_t pin_number;

    while(1) {
        if(xQueueReceive(interruptQueue, &pin_number, portMAX_DELAY)) {

            // NFC Tag
            if(pin_number == GPIO_NUM_5) {
                #if DEBUG
                printf("Detected NFC-Tag Scan!\n");
                #endif
                if(deviceControl->btHandler->a2dpController.isConnected() || deviceControl->btHandler->a2dpController.abnormalDisconnection()) {
                    #if DEBUG
                    printf("Removing current owner!\n");
                    #endif
                    gpio_intr_disable(GPIO_NUM_5); // Disable gpio intr to ensure that it only interrupts once
                    deviceControl->nfc_tag_scanned = true;
                    uart_write_bytes(UART_NUM_0, "$BLINK\n", strlen("$BLINK\n"));


                    if(deviceControl->btHandler->a2dpController.abnormalDisconnection()) { // Allow new remote device connection when last device got abnormal disconnect
                        deviceControl->zeroOwner();
                    }

                    memcpy(deviceControl->btHandler->last_connected_owner, deviceControl->btHandler->owner, ESP_BD_ADDR_LEN);
                    
                    #if DEBUG
                    printf("Disconnected Owner: ");
                    for(int i = 0; i < ESP_BD_ADDR_LEN; i++){
                        printf("%02X ", deviceControl->btHandler->owner[i]);
                    }
                    printf("\n");
                    #endif

                    esp_a2d_sink_disconnect(deviceControl->btHandler->owner);
                }
                else {
                    #if DEBUG
                    printf("No device is connected!\n");
                    #endif
                    gpio_intr_disable(GPIO_NUM_5);
                    deviceControl->nfc_tag_scanned = true;
                    uart_write_bytes(UART_NUM_0, "$BLINK\n", strlen("$BLINK\n"));
                    
                    // if (deviceControl->btHandler->first_time_boot) {
                    //     esp_a2d_sink_disconnect(deviceControl->btHandler->owner);
                    //     deviceControl->zeroOwner();
                    //     deviceControl->btHandler->first_time_boot = false;
                    // }
                    
                    deviceControl->btHandler->setDiscoverable(true);
                }   
            }

            // Button is pressed
            if(pin_number == GPIO_NUM_36) {
                #if DEBUG
                printf("Button was pressed!\n");
                #endif
                gpio_intr_disable(GPIO_NUM_36);
                uint32_t pinlevel = gpio_get_level(GPIO_NUM_36);
                ICESleep(1500);
                uint32_t pinlevelNow = gpio_get_level(GPIO_NUM_36);
                
                if (pinlevel == 1 && pinlevelNow == 1) {

                    if (deviceControl->btHandler->first_time_boot) {
                        deviceControl->btHandler->first_time_boot = false;
                    }
                    #if DEBUG
                    QPrint::println("Button held for 2 seconds!");
                    #endif
                    memcpy(deviceControl->btHandler->last_connected_owner, deviceControl->btHandler->owner, ESP_BD_ADDR_LEN);
                    deviceControl->pairing_mode = true;
                    uart_write_bytes(UART_NUM_0, "$BLINK\n", strlen("$BLINK\n"));
                    if(deviceControl->btHandler->a2dpController.isConnected()){
                        // Removing connected device and initiating pairing mode
                        esp_a2d_sink_disconnect(deviceControl->btHandler->owner);  
                    }
                    else {
                        // Initiating pairing mode
                        deviceControl->btHandler->setDiscoverable(true);
                    }

                }

                gpio_intr_enable(GPIO_NUM_36);
            }
        } 
    }
}



static void app_task(void* arg) {

    
    while(1) {

        // QPrint::println("Free heap size: " + std::to_string(xPortGetFreeHeapSize()));
        // QPrint::println("Minimum ever heap: " + std::to_string(xPortGetMinimumEverFreeHeapSize()));
        
        // deviceControl->update_LEDC_variables(tflag);
        // deviceControl->UARTControlMain();

        // if no button press and nfc tag scan
        if(!deviceControl->nfc_tag_scanned && !deviceControl->pairing_mode && !deviceControl->btHandler->first_time_boot) {
            
            if(deviceControl->timer_started) {
                esp_timer_stop(deviceControl->timer);
                deviceControl->timer_started = false;
            }
            
            if(deviceControl->connected_after_nfc) { 
                esp_timer_start_once(deviceControl->timer, 5000000);
            }
            
            #if DEBUG
            printf("Bluetooth Owner: ");
            for(int i = 0; i < ESP_BD_ADDR_LEN; i++) {
                printf("%02X ", deviceControl->btHandler->owner[i]);
            }
            printf("\n");
            #endif 
            

            deviceControl->btHandler->reconnect(); // If signal loss try to reconnect

        }
        else { // if button press or nfc tag scan
            
            // Upon first boot attempt to reconnect to last connected device for 15 seconds
            if (deviceControl->btHandler->first_time_boot) {
                if(esp_timer_start_once(deviceControl->timer, 15000000) == ESP_OK){
                    deviceControl->timer_started = true;
                    #if DEBUG
                    printf("Attempting to reconnect to last connected owner...\n");
                    #endif
                }
                deviceControl->btHandler->reconnect();
            }
            else {
                 
                if(deviceControl->nfc_tag_scanned) { /* 10 seconds to connect on NFC tag */
                    if(esp_timer_start_once(deviceControl->timer, 10000000) == ESP_OK) {
                        deviceControl->timer_started = true;
                    }
                }
                else if(deviceControl->pairing_mode) { /* 30 seconds to connect via button */
                    if(esp_timer_start_once(deviceControl->timer, 30000000) == ESP_OK) {
                        deviceControl->timer_started = true;
                        #if DEBUG
                        printf("Started timer!\n");
                        #endif
                    }
                }
            }
        }
        ICESleep(10);
    }
}

// Callback function for timer
static void connection_timer(void* arg) {

    if(deviceControl->pairing_mode) { // If no device connects during pairing mode
        deviceControl->pairing_mode = false;
        deviceControl->btHandler->setDiscoverable(false);
        #if DEBUG
        printf("30 seconds have passed since pairing mode was initiated and no device connected!\n");
        #endif
    }
    
    if(deviceControl->btHandler->first_time_boot) { // Remote device fails to reconnect after 15 seconds after boot
        deviceControl->zeroOwner();
        deviceControl->btHandler->setDiscoverable(false);
        deviceControl->btHandler->first_time_boot = false;
        #if DEBUG
        printf("Last owner did not manage to reconnnect upon boot!\n");
        #endif
    }
    
    if(deviceControl->nfc_tag_scanned) { // If no device connected during nfc tag scan
        gpio_intr_enable(GPIO_NUM_5);
        deviceControl->nfc_tag_scanned = false;
        deviceControl->btHandler->setDiscoverable(false);
        #if DEBUG
        printf("15 seconds have passed since last device was connected. Enabled interrupt again!\n");
        #endif
    }

    if(deviceControl->connected_after_nfc) { // Enable intr after 10 second delay after nfc connect
        gpio_intr_enable(GPIO_NUM_5);
        deviceControl->connected_after_nfc = false;
    }
    else {
        uart_write_bytes(UART_NUM_0, "$FAILED\n", strlen("$FAILED\n"));
        memset(deviceControl->btHandler->last_connected_owner, 0, ESP_BD_ADDR_LEN);
    }
}


void DeviceControl::start_tasks() {

    // Connection timer for timer callback function
    
    esp_timer_create_args_t timer_args = {
        .callback = &connection_timer,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "one_shot" 
    };

    // Create esp timer instance
    if(esp_timer_create(&timer_args, &timer) != ESP_OK) {
        printf("Failed to create timer!\n");
    }   


    // Create queue with size of 1
    interruptQueue = xQueueCreate(1, sizeof(int));

    // Install isr
    if(gpio_install_isr_service(0) != ESP_OK) {
        printf("Failed to install isr service!\n");
    }

    // Add ISR handler for gpio 5
    if(gpio_isr_handler_add(GPIO_NUM_5, gpio_interrupt_handler, (void*) GPIO_NUM_5) != ESP_OK) {
        printf("Failed to add isr handler for gpio 5\n");
    }

    // Add ISR handler for gpio 36
    if(gpio_isr_handler_add(GPIO_NUM_36, gpio_interrupt_handler, (void*) GPIO_NUM_36) != ESP_OK){
        printf("Failed to add isr handler for gpio 36\n");
    }
    
    xTaskCreate(app_task, "app_task", 2048, NULL, 1, NULL);
    xTaskCreate(set_new_owner, "set_new_owner", 2048, NULL, 1, NULL);
}

void DeviceControl::print(std::string input) {
    switch(deviceState) {
        case DEVICE_IDLE:
            processIdleInput(input);
            break;
        /*case DEVICE_CHOOSING_WIFI:
            processWiFiChoice(input);
            break;
        case DEVICE_ENTER_WIFI_PASS:
            processWiFiPass(input);
            break;
        */
    }
}

void DeviceControl::processIdleInput(std::string input) {
    std::vector<std::string> tokenizedInput = ICETokenize(input, " ");
    /*if (tokenizedInput.front() == "SCANWIFI") {
        wifiNetworks = scanWifis(&foundWiFis);
        if (foundWiFis == 0) {
            QPrint::println("No WiFi networks were found... :/", controlOutput, true);
            return;
        }
        printWifis();
        deviceState = DEVICE_CHOOSING_WIFI;
    } 
    else if (tokenizedInput.front() == "STREAM") {
        if (wiFiHandler == nullptr || !wiFiHandler->isConnected()) {
            QPrint::println("Not connected to WiFi. Please connect to WiFi before starting a stream. Wait a few seconds for WiFi to connect.", controlOutput, true);
        }
        if (tokenizedInput.size() <= 1) {
            QPrint::println("No stream link provided. Please input link", controlOutput, true);
            return;
        }
        stopRadioStream(); //Make sure if we were playing radio, to stop the old one...
        startRadioStream(tokenizedInput.at(1));
        QPrint::println("Command OK", controlOutput, true);
    } else if (tokenizedInput.front() == "STREAMNO") {
        if (wiFiHandler == nullptr || !wiFiHandler->isConnected()) {
            QPrint::println("Not connected to WiFi. Please connect to WiFi before starting a stream. Wait a few seconds for WiFi to connect.", controlOutput, true);
        }

        if (tokenizedInput.size() <= 1) {
            QPrint::println("No Number provided. List of channels are:", controlOutput, true);
            QPrint::println("1: DR P1", controlOutput, true);
            QPrint::println("2: Romanian radio station", controlOutput, true);
            QPrint::println("3: DR P3", controlOutput, true);
            return;
        }

        stopRadioStream(); //Make sure if we were playing radio, to stop the old one...
        if (tokenizedInput.at(1) == "1") {
            startRadioStream("http://live-icy.gslb01.dr.dk/A/A03L.mp3");
        } else if (tokenizedInput.at(1) == "2") {
            startRadioStream("http://89.205.123.70/;?type=http&nocache=26141");
        } else if (tokenizedInput.at(1) == "3") {
            startRadioStream("http://live-icy.gslb01.dr.dk/A/A05L.mp3");
        } else {
            QPrint::println("Invalid number", controlOutput, true);
            return;
        }
        QPrint::println("OK", controlOutput, true);
    } */
    if (tokenizedInput.front() == "BLUETOOTH") {
        //stopRadioStream();
        ICESleep(800); //Give some time for I2S to clear buffers and such
        btHandler->startAudio();
        QPrint::println("OK", controlOutput, true);
    } else if (tokenizedInput.front() == "PLAY") {
        if (btHandler->avrcController.play()) {
            QPrint::println("OK", controlOutput, true);
        } else {
            QPrint::println("NOT OK", controlOutput, true);
        }
    } else if (tokenizedInput.front() == "PAUSE") {
        if (btHandler->avrcController.pause()) {
            QPrint::println("OK", controlOutput, true);
        } else {
            QPrint::println("NOT OK", controlOutput, true);
        }
    } else if (tokenizedInput.front() == "NEXT") {
        if (btHandler->avrcController.next()) {
            QPrint::println("OK", controlOutput, true);
        } else {
            QPrint::println("NOT OK", controlOutput, true);
        }
    } else if (tokenizedInput.front() == "PREV") {
        if (btHandler->avrcController.previous()) {
            QPrint::println("OK", controlOutput, true);
        } else {
            QPrint::println("NOT OK", controlOutput, true);
        }
    } else if (tokenizedInput.front() == "HELP") {
        QPrint::println("SCANWIFI\nSTREAM\nSTREAMNO\nBLUETOOTH\nNEXT\nPREV\nPAUSE\nPLAY\nHELP", controlOutput, true);
    } else {
        QPrint::println("Invalid command?", controlOutput, true);
    }
}

void DeviceControl::setBluetoothOwner(esp_bd_addr_t deviceAddress) {
    nvs.setBinaryValue("BluetoothOwner", deviceAddress, ESP_BD_ADDR_LEN);
    btHandler->setDiscoverable(false);
}

void DeviceControl::removeBluetoothOwner() {
    uint8_t zero[ESP_BD_ADDR_LEN];
    memset(zero, 0, ESP_BD_ADDR_LEN);
    nvs.setBinaryValue("BluetoothOwner", zero, ESP_BD_ADDR_LEN);
    //btHandler->setDiscoverable(true);
}

void DeviceControl::zeroOwner() {
    static uint8_t zero[ESP_BD_ADDR_LEN] = {0, 0, 0, 0, 0, 0};
    removeBluetoothOwner();
    btHandler->setOwner(zero);
}

void DeviceControl::newOwner(esp_bd_addr_t new_owner) {
    setBluetoothOwner(new_owner); // Set BT owner in nvs partition & and set discoverable to false
    btHandler->setOwner(new_owner); // Set owner for reconnect function
    #if DEBUG
    printf("A2DP was connected to: ");
    for(int i = 0; i < ESP_BD_ADDR_LEN; i++){
        printf("%02X ", new_owner[i]);
    }
    printf("\n");
    #endif
}

bool DeviceControl::preventAutoreconnect(esp_bd_addr_t connecting_owner) {
    // Prevent the last nfc connected remote device that was disconnected from reconnecting straight away (Prevents the connection to the esp32 from becoming a race to connect)
    if(memcmp(connecting_owner, btHandler->last_connected_owner, ESP_BD_ADDR_LEN) == 0 
                && (nfc_tag_scanned || pairing_mode) && !btHandler->a2dpController.abnormalDisconnection()) { 
        esp_a2d_sink_disconnect(connecting_owner);

        #if DEBUG
        printf("Preventing disconnecting remote device from reconnecting: ");
        for(int i = 0; i < ESP_BD_ADDR_LEN; i++) {
            printf("%02X ", connecting_owner[i]);
        }
        printf("\n");
        #endif

        return true;
    }
    return false;
}

void DeviceControl::update_LEDC_variables(bool flag){
    update_bluetooth_state_LEDC_timer();
    if (tflag_old == flag){
        buttonCheck();
        buttonIndicatorLEDTimer();

        timer_cnt++;
        tflag_old = !flag;
        if ((timer_cnt & LEDC_timer_bitmask) == LEDC_timer_bitmask)
        {
            ledc_controller.setBrightness(100);
        }
        else
        {
            ledc_controller.setBrightness(0);
        }
        
    }
    ledc_controller.updatePWM();
}



void DeviceControl::set_bluetooth_state_flag(uint8_t state){
    /**
     * Function sets the internal variable deviceBluetoothState to reflect the state of the bluetooth.
     * 0 = PAIRING_MODE
     * 1 = CONNECTED
     * 2 = NOT_CONNECTED_NOT_PAIRING
     * 3 = NO_DSP_NO_MIXAMP
     * 4 = BUTTON_PRESS
     * 5 = BUTTON_PRESS_DONE
    */

    switch (state)
    {
    case 0:
        deviceBluetoothState = PAIRING_MODE;
        break;
    
    case 1:
        deviceBluetoothState = CONNECTED;
        break;

    case 2:
        deviceBluetoothState = NOT_CONNECTED_NOT_PAIRING;
        break;

    case 3:
        deviceBluetoothState = NO_DSP_NO_MIXAMP;
        break;

    case 4:
        deviceBluetoothState = BUTTON_PRESS;
        break;

    case 5:
        deviceBluetoothState = BUTTON_PRESS_DONE;
        break;

    default:
        break;
    }
}

void DeviceControl::update_bluetooth_state_LEDC_timer(){
    //checks the bluetooth state and accordingly sets the LED color and timing
    switch (deviceBluetoothState)
    {
    case PAIRING_MODE:
        LEDC_timer_bitmask = 0b00001000;
        ledc_controller.setRGB('B');
        break;

    case CONNECTED:
        LEDC_timer_bitmask = 0b00000000;
        ledc_controller.setRGB('B');
        break;

    case NOT_CONNECTED_NOT_PAIRING:
        LEDC_timer_bitmask = 0b00000100;
        ledc_controller.setRGB('B');
        break;
        
    case NO_DSP_NO_MIXAMP:
        LEDC_timer_bitmask = 0b00000000;
        ledc_controller.setRGB('R');
        break;
    
    case BUTTON_PRESS:
        LEDC_timer_bitmask = 0b00000010;
        ledc_controller.setRGB('G');
        break;

    case BUTTON_PRESS_DONE:
        LEDC_timer_bitmask = 0b00000001;
        ledc_controller.setRGB('W');
        break;

    default:
        break;
    }     
}

void DeviceControl::buttonCheck() {
    if(!disconnectFlag)
    {
        if (gpio_get_level(GPIO_BUTTON_PIN))
        {
            if(buttoncnt == 0)
            {
                previousdeviceBluetoothState = deviceBluetoothState;
            }
            
            buttoncnt++;
            if(deviceBluetoothState != BUTTON_PRESS)
            {
                set_bluetooth_state_flag(BUTTON_PRESS);
            }
        }
        else
        {
            buttoncnt = 0;
            set_bluetooth_state_flag(previousdeviceBluetoothState);
        }
    }
    
    if (buttoncnt>BUTTON_PRESS_TIME && !disconnectFlag)
    {
        removeBluetoothOwner();
        buttoncnt = 0;
        disconnectFlag = true;
    }
}

void DeviceControl::buttonIndicatorLEDTimer(){
    if(disconnectFlag)
    {
        buttoncnt++;
        set_bluetooth_state_flag(5);
        QPrint::println("buttonCNT:" + std::to_string(buttoncnt));
        if(buttoncnt>5)
        {
            QPrint::println("HERE");
            disconnectFlag = false;
            set_bluetooth_state_flag(0);
            buttoncnt = 0;
        }
    }
}