#include "LEDDriver.hpp"

LEDDriver::LEDDriver(){
    //setup code here
    ledc_timer_config_t ledc_timer0 = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer0);
    ledc_channel0.gpio_num = LEDC_BLUE_PIN;
    ledc_channel0.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel0.channel = LEDC_CHANNEL_0;
    ledc_channel0.timer_sel = LEDC_TIMER_0;
    ledc_channel0.duty = 0;
    ledc_channel0.hpoint = 0;
    ledc_channel_config(&ledc_channel0);


    ledc_timer_config_t ledc_timer1 = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_1,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer1);
    ledc_channel1.gpio_num = LEDC_RED_PIN;
    ledc_channel1.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel1.channel = LEDC_CHANNEL_1;
    ledc_channel1.timer_sel = LEDC_TIMER_1;
    ledc_channel1.duty = 0;
    ledc_channel1.hpoint = 0;
    ledc_channel_config(&ledc_channel1);

    ledc_timer_config_t ledc_timer2 = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_2,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer2);
    ledc_channel2.gpio_num = LEDC_GREEN_PIN;
    ledc_channel2.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel2.channel = LEDC_CHANNEL_2;
    ledc_channel2.timer_sel = LEDC_TIMER_2;
    ledc_channel2.duty = 0;
    ledc_channel2.hpoint = 0;
    ledc_channel_config(&ledc_channel2);

    brightness = 100;
}

void LEDDriver::setPWMPercent(ledc_channel_config_t* ledconf, uint8_t percent) {
    uint32_t value = (1023.f / 100.f) * percent;

    ledc_set_duty(ledconf->speed_mode, ledconf->channel, value);
    ledc_update_duty(ledconf->speed_mode, ledconf->channel);
}

void LEDDriver::setBrightness(uint8_t value){
    if(value<=100)
        brightness = value;
    else
        return;
}

void LEDDriver::setRGB(uint8_t R, uint8_t G, uint8_t B){
    RGB[0] = R;
    RGB[1] = G;
    RGB[2] = B;
}

void LEDDriver::setRGB(char color){
    switch (color)
    {
    case 'R':
        setRGB(255,0,0);
        break;
    
    case 'G':
        setRGB(0,255,0);
        break;
    
    case 'B':
        setRGB(0,0,255);
        break;
    
    case 'C':
        setRGB(0, 255, 255);
        break;

    case 'M':
        setRGB(255, 0, 255);
        break;

    case 'Y':
        setRGB(255, 255, 0);
        break;

    case 'W':
        setRGB(255, 255, 255);
        break;

    case ' ':
        setRGB(0, 0, 0);
        break;

    default:
        setRGB(0, 0, 0);
        break;
    }

}

void LEDDriver::updatePWM(){
    //0 = max brightness, 100 = min brightness 
    setPWMPercent(&ledc_channel0, 100-((RGB[2]/255)*brightness)); //Blue channel PWM control
    setPWMPercent(&ledc_channel1, 100-((RGB[0]/255)*brightness)); //Red channel PWM control
    setPWMPercent(&ledc_channel2, 100-((RGB[1]/255)*brightness)); //Green channel PWM control
}