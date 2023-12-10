#include "DeviceControl.hpp"




// // PWM for LED
// void blink_led(ledc_channel_config_t* ledconf) {
//     ledc_set_fade_with_time(ledconf->speed_mode, ledconf->channel, 4000, 500);
//     ledc_fade_start(ledconf->speed_mode, ledconf->channel, LEDC_FADE_NO_WAIT);

//     ledc_set_fade_with_time(ledconf->speed_mode, ledconf->channel, 0, 500);
//     ledc_fade_start(ledconf->speed_mode, ledconf->channel, LEDC_FADE_NO_WAIT);
// }

// // debug
// void print_bt_owner(uint8_t *owner, char* text){
//     printf("%s", text);
//     for(int i = 0; i < ESP_BD_ADDR_LEN; i++) {
//         printf("%02X ", owner[i]);
//     }
//     printf("\n");
// }

DeviceControl* deviceController;

extern "C" void app_main(void)
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);


    deviceController = new DeviceControl();
   
    // uint16_t millis = 1000;
    // ICETask* LEDC_timer_callback = new TimerTaskCallback();
    // ICETimer LEDC_timer = {"LEDTimer", millis*0.2, true, LEDC_timer_callback};
    // LEDC_timer.start();

    // Create task that handles setting a new owner via nfc tag or button press
    


    while(1) {
        ICESleep(1000);
    }

}
