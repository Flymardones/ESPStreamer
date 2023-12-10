#pragma once

#include "GAPControl.hpp"
#include "SPPControl.hpp"
#include "A2DPControl.hpp"
#include "AVRCControl.hpp"

//The different callbacks
void GAPCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *parameter);
void SPPCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *parameter);



class BluetoothCallbackManager {
    private:
        static GAPControl* gapController;
        static SPPControl* sppController;
        static A2DPControl* a2dpController;
        static AVRCControl* avrcController;
        
    public:
        static void setGAPController(GAPControl* controller);
        static void setSPPController(SPPControl* controller);
        static void setA2DPController(A2DPControl* controller);
        static void setAVRCController(AVRCControl* controller);

        static int registerCallbacks();
        static void GAPCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* parameter);
        static void SPPCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t* parameter);
        static void A2DPControlCallback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t* parameter);
        static void A2DPDataCallback(const uint8_t *data, uint32_t size);
        static void AVRCCallback(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t* parameter);
};