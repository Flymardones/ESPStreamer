#pragma once

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ICEString.hpp"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "NVS.hpp"
#include "BluetoothHandler.hpp"


#define BAUD_RATE 115200
#define BUF_SIZE 512
#define UART_TX_PIN 1
#define UART_RX_PIN 3

#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION 0
#endif




class UARTHandler {
    private:
    uint8_t length;
    NVS nvs;


    public:
    // UARTHandler(NVS& nvs, BluetoothHandler& btHandler) : nvs(nvs), btHandler(btHandler) {}
    // NVS &nvs;
    // BluetoothHandler &btHandler;
    static UARTHandler& GetInstance() { static UARTHandler uartHandler; return uartHandler; }
    const uart_port_t uart_num = UART_NUM_0;
    UARTHandler();
    ~UARTHandler();
    void handleQuery(std::string query);
    void readUART();
    uint8_t writeUART(char *input);


};