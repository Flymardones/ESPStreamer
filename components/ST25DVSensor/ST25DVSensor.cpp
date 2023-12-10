/**
  ******************************************************************************
  * @file    ST25DVSensor.c
  * @author  MCD Application Team
  * @brief   Source file of NFC ST25SV sensor module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "ST25DVSensor.h"
// #include "driver/gpio.h"
// #include "driver/i2c.h"

int ST25DV::begin()
{
    uint8_t nfctag_id = 0;

    if (!NfctagInitialized) {
        /* ST25DV Init */
        // if (ST25DV_Init() != NFCTAG_OK) {
        //     return NFCTAG_ERROR;
        // }
       
        /* Check ST25DV driver ID */
        st25dv_io.ST25DV_i2c_ReadID(&nfctag_id);

        if ((nfctag_id == I_AM_ST25DV04) || (nfctag_id == I_AM_ST25DV64) ||
            (nfctag_id == I_AM_ST25DV04KC) || (nfctag_id == I_AM_ST25DV64KC)) {
            NfctagInitialized = 1;
        } else {
            return NFCTAG_ERROR;
        }

        int ret = ndef.begin();
        if (ret != NDEF_OK) {
            return ret;
        }
    }
    return NFCTAG_OK;
}

int ST25DV::writeURI(const char *protocol, const char *uri, const char *info)
{
    sURI_Info _URI;

    if(strcmp(protocol, "") == 0){
        return NDEF_ERROR;
    }

    strncpy(_URI.protocol, protocol, sizeof(_URI.protocol));
    strncpy(_URI.URI_Message, uri, sizeof(_URI.URI_Message));
    strncpy(_URI.Information, info, sizeof(_URI.Information));

    return ndef.NDEF_WriteURI(&_URI);
}

int ST25DV::readURI(char *s)
{
    uint16_t ret;
    sURI_Info uri = {"", "", ""};
    sRecordInfo_t recordInfo;
    uint8_t NDEF_Buffer[100];

    ret = ndef.NDEF_ReadNDEF(NDEF_Buffer);
    if (ret) {
        printf("NDEF_ReadNDEF failed!\n");
        return ret;
    }
 

    ret = ndef.NDEF_IdentifyBuffer(&recordInfo, NDEF_Buffer);
    if (ret) {
        printf("NDEF_IdentifiyBuffer failed\n");
        return ret;
    }
  

    ret = ndef.NDEF_ReadURI(&recordInfo, &uri);
    if (ret) {
        printf("NDEF_ReadURI failed\n");
        return ret;
    }
   
    strcpy(s, uri.protocol);
    strcat(s, uri.URI_Message);
   
    return 0;
}

int ST25DV::writeBluetoothOOB(Ndef_Bluetooth_OOB_t *pBluetooth, char *RecordID) {
    uint16_t ret;

    ret = ndef.NDEF_ClearNDEF();
    if (ret) {
        printf("Failed to clear NDEF!\n");
        return ret;
    }

    ret = ndef.NDEF_AppendBluetoothOOB(pBluetooth, RecordID);
    if (ret) {
        printf("Failed to append bluetooth OOB!\n");
    }
   
    
    return 0;
}

NDEF *ST25DV::getNDEF(void)
{
    return &ndef;
}



// NFCTAG_StatusTypeDef ST25DV::ST25DV_Init(void)
// {
//     if (st25dv_io.get_pwire() == NULL) {
//         return NFCTAG_ERROR;
//     }

//     ST25DV_GPO_Init();
//     ST25DV_LPD_Init();

//     ST25DV_I2C_Init();
//     ST25DV_SelectI2cSpeed(3);

//     return NFCTAG_OK;
// }

// void ST25DV::ST25DV_GPO_Init(void)
// {
//     gpio_set_direction(st25dv_io.get_gpo(), GPIO_MODE_INPUT);
// }

// uint8_t ST25DV::ST25DV_GPO_ReadPin(void)
// {
//     return gpio_get_level(st25dv_io.get_gpo());
// }

// void ST25DV::ST25DV_LPD_Init(void)
// {
//     if (st25dv_io.get_lpd() > 0) {
//         gpio_set_direction(st25dv_io.get_lpd(), GPIO_MODE_OUTPUT);
//         gpio_set_level(st25dv_io.get_lpd(), 0);
//     }
// }

// void ST25DV::ST25DV_LPD_DeInit(void)
// {
//     // You can add any de-initialization code here if needed
// }

// uint8_t ST25DV::ST25DV_LPD_ReadPin(void)
// {
//     return gpio_get_level(st25dv_io.get_lpd());
// }

// void ST25DV::ST25DV_LPD_WritePin(uint8_t LpdPinState)
// {
//     gpio_set_level(st25dv_io.get_lpd(), LpdPinState);
// }

// void ST25DV::ST25DV_SelectI2cSpeed(uint8_t i2cspeedchoice)
// {
//     // ESP-IDF doesn't provide a direct way to change I2C speed on-the-fly.
//     // If you need to change the I2C speed, you'll have to de-initialize and then re-initialize the I2C driver with the new speed.
// }

// void ST25DV::ST25DV_I2C_Init(void)
// {
//     // ESP-IDF I2C initialization code would go here.
//     // This would involve setting up an i2c_config_t struct and then calling i2c_param_config() and i2c_driver_install().
// }

