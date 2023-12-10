#include "driver/i2s_std.h"
#include "driver/gpio.h"


#pragma once

class I2SOutput {
    private:
        
        i2s_chan_handle_t tx_handle;
        i2s_std_config_t std_cfg;
        i2s_data_bit_width_t bitWidth = I2S_DATA_BIT_WIDTH_16BIT;
        uint32_t sampleRate = 44100;
        i2s_slot_mode_t channelMode = I2S_SLOT_MODE_STEREO;

        void configureI2S();

    public:
        // void runTask(void* unused);
        I2SOutput();
        ~I2SOutput();

        void setSampleRate(uint32_t sampleRate);
        int getSampleRate();
        bool writeDirect(const uint8_t* data, uint32_t size);
};