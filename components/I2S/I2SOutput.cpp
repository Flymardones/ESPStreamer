#include "I2SOutput.hpp"
#include "ICESleep.hpp"
#include "QPrint.hpp"
#include "cstring"

I2SOutput::I2SOutput() {
    printf("I2SOutput Constructor\n");
    configureI2S();
}

I2SOutput::~I2SOutput() {
    //i2s_driver_uninstall(I2SPort);
    i2s_del_channel(tx_handle);
}

void I2SOutput::configureI2S() {
    //The DMA buffer is set to the same size as the ringbuffer, 4*1024*16/8 = 16k.
    //This allows us to hold 85ms of audio at 48khz.

    i2s_chan_config_t chan_cfg = {
        .id = I2S_NUM_0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 8,
        .dma_frame_num = 64,
        .auto_clear = true,
    };


    if (i2s_new_channel(&chan_cfg, &tx_handle, NULL) != ESP_OK) {
        printf("Failed to create I2S channel\n");
        return;
    }
    
    std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sampleRate),
        .slot_cfg = I2S_STD_PCM_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, channelMode), // PCM or Phillips
        .gpio_cfg = {
            .mclk = GPIO_NUM_0,
            .bclk = GPIO_NUM_4,
            .ws = GPIO_NUM_2,
            .dout = GPIO_NUM_15,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    if (i2s_channel_init_std_mode(tx_handle, &std_cfg) != ESP_OK) {
        printf("Failed to init I2S channel\n");
        return;
    }

    if (i2s_channel_enable(tx_handle) != ESP_OK) {
        printf("Failed to enable I2S channel\n");
        return;
    }
}

void I2SOutput::setSampleRate(uint32_t sampleRate) {
    if (sampleRate > 8000 && sampleRate < 100000) { //Sanity check
        this->sampleRate = sampleRate;
        i2s_channel_disable(tx_handle);
        std_cfg.clk_cfg.sample_rate_hz = sampleRate;
        i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg);
        i2s_channel_enable(tx_handle);
    }
}

int I2SOutput::getSampleRate() {
    return this->sampleRate;
}

bool I2SOutput::writeDirect(const uint8_t *data, uint32_t size) {
    size_t writtenBytes = 0;

    esp_err_t status = i2s_channel_write(tx_handle, data, size, &writtenBytes, portMAX_DELAY);
    //i2s_write(I2SPort, data, size, &writtenBytes, 1); //Something is quite wrong with clocking if we end up waiting here..
    if (status != ESP_OK) {
        return false;
    }

    return true;
}
