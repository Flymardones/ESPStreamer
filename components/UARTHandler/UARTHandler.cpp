#include "UARTHandler.hpp"

static QueueHandle_t uart_queue;


static void uart_event_handler(void *pvParameters) {
    uart_event_t event;


    while (1) {
        if(xQueueReceive(uart_queue, (void *)&event, (TickType_t) portMAX_DELAY)) {

            if(event.type == UART_DATA) {

                char* data = (char *) malloc(event.size);

                if (data == NULL) {
                    printf("Failed to allocate mem!\n");
                }

                // Read data from UART
                if (uart_read_bytes(UART_NUM_0, data, event.size, portMAX_DELAY) < 0) {
                    printf("Failed to read data from UART!\n");
                }
                                
                UARTHandler::GetInstance().handleQuery((std::string) data);
                
                free(data);
                data = NULL;
            }
        }
    }
}

void UARTHandler::handleQuery(std::string query) {
    std::vector<std::string> tokenizedInput = ICETokenize(query, ",");

    if (!tokenizedInput.empty()) {
        if (tokenizedInput.at(0) == "FIRMWARE_VERSION") {
            // Convert int to char *
            std::string s = std::to_string(FIRMWARE_VERSION);
            s += '\n';
            const char * firmware_version = ('$' + s).c_str();

            if(uart_write_bytes(uart_num, firmware_version,  strlen(firmware_version)) < 0) { 
                printf("UART failed to write!\n");
            }
        }
        else if (tokenizedInput.at(0) == "DEVICE_NAME") {

            if (BluetoothHandler::GetInstance().a2dpController.isConnected()) {
                uart_write_bytes(uart_num, "$CONNECTED\n", strlen("$CONNECTED\n"));
            }

            std::string current_device_name = NVS::GetInstance().getStringValue("BTDeviceName");
            std::string device_name = tokenizedInput.at(1);
            
            /* Change ESP device name if GUI device name has changed */
            if (device_name != current_device_name) {
                NVS::GetInstance().setStringValue("BTDeviceName", device_name);
                BluetoothHandler::GetInstance().setDeviceName(device_name);
            }            
        }
    }
}



UARTHandler::UARTHandler() {
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    if (uart_driver_install(uart_num, BUF_SIZE, BUF_SIZE, 10, &uart_queue, 0) != ESP_OK) {
        printf("Failed to install UART driver!\n");
    }

    if (uart_param_config(uart_num, &uart_config) != ESP_OK) {
        printf("Failed to set uart config!\n");
    }

    // Using UART0 default pins
    if (uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        printf("Failed to set pins!\n");
    }

    xTaskCreate(uart_event_handler, "uart_event_handler", 2048, NULL, 12, NULL);
    printf("UARTHandler Constructor\n");
}

UARTHandler::~UARTHandler() {
    uart_driver_delete(uart_num);
    vTaskDelete(NULL);
}

// void UARTHandler::readUART()
// {
//     ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
//     if(length<=BUF_SIZE){
//         length = uart_read_bytes(uart_num, data_UART, length, 100);
//     }

// }

// uint8_t UARTHandler::writeUART(char* input)
// {
//     uart_write_bytes(uart_num, "\n\r", strlen("\n\r"));
//     return uart_write_bytes(uart_num, input, strlen(input));
// }


// void DeviceControl::readUARTCommands(){
//     std::string cmd = (char *) uartHandler.getStoredData();
    
//     std::size_t found =cmd.find_last_of("\n\r");//remove enter
//     cmd = cmd.substr(0,found);
//     QPrint::println(cmd);

//     std::vector<std::string> tokenizedInput = ICETokenize(cmd, " ");

    
//     if (cmd.substr(0,uartHandler.getLength() - 1) ==  std::string("BT_pause"))
//     {
//         uartHandler.writeUART("stopping BT audio");
//         //QPrint::println("\nStopping Bluetooth Audio");
//         btHandler->stopAudio();
//     }
//     else if (cmd.substr(0,uartHandler.getLength() - 1 ) ==  std::string("BT_play"))
//     {
//         uartHandler.writeUART("starting BT audio");
//         //QPrint::println("\nStarting Bluetooth Audio");
//         btHandler->startAudio();
//     }
//     else if(cmd.substr(0,uartHandler.getLength() - 1 ) ==  std::string("BT_remDev"))
//     {
//         uartHandler.writeUART("removing device");
//         //QPrint::println("\nRemoving Owner");
//         removeBluetoothOwner(); //does not do anything?      
//     }
//     else if (cmd.substr(0,uartHandler.getLength() - 1 ) ==  std::string("BT_disc"))
//     {
//         btHandler->setDiscoverable(true);
//         uartHandler.writeUART("setting device to discoverable");
//     }
//     else if (cmd.substr(0,uartHandler.getLength() - 1 ) ==  std::string("BT_start"))//this should not be called more than once...
//     {
//         startBluetooth();
//     }
//     else if (cmd.substr(0,uartHandler.getLength() - 1 ) ==  std::string("help"))
//     {
//         uartHandler.writeUART("Commands \tDescription\n\r");
//         uartHandler.writeUART("BT_play  \tPlay audio on paired bluetooth device");
//         uartHandler.writeUART("BT_pause \tPause audio on paired bluetooth device");
//         uartHandler.writeUART("BT_disc  \tSet Bluetooth device to be discoverable");
//         uartHandler.writeUART("BT_remDev\tRemove currently connected device from bluetooth");
//     }
//     else
//     {
//         uartHandler.writeUART("No valid input. Type \"help\" for available commands");
//         QPrint::println("\nNo valid input"); 
//         QPrint::println(cmd.substr(0,uartHandler.getLength()));
//         QPrint::println(std::to_string(uartHandler.getLength()));
//     }
//     uartHandler.writeUART("\n\r");

// }
