#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#define LEDC_BLUE_PIN 32 
#define LEDC_RED_PIN 33 
#define LEDC_GREEN_PIN 25 

class LEDDriver{
    uint8_t duty_cycle;
    uint8_t brightness;
    uint8_t RGB[3] = {255, 255, 255};

public:
    ledc_channel_config_t ledc_channel0 = {}; //encodes Blue channel
    ledc_channel_config_t ledc_channel1 = {}; //encodes Red channel
    ledc_channel_config_t ledc_channel2 = {}; //encodes Green channel
    
    LEDDriver();
    void setPWMPercent(ledc_channel_config_t* ledconf, uint8_t percent);
    void setBrightness(uint8_t value);

    void setRGB(uint8_t R, uint8_t G, uint8_t B);
    void setRGB(char color);

    void updatePWM();

};