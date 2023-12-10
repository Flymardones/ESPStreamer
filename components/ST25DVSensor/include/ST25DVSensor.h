/**
  ******************************************************************************
  * @file    ST25DVSensor.h
  * @author  MCD Application Team
  * @brief   Header file of NFC ST25SV sensor module.
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
#ifndef _ST25DVSENSOR_H_
#define _ST25DVSENSOR_H_


#include <stdint.h>
//#include "driver/i2c.h"
#include "st25dv_io.h"
#include "NDEF_class.h"

class ST25DV {
public:
    ST25DV() : st25dv_io(), ndef() {}
    
    int begin();
    int begin(uint8_t *buffer, uint16_t bufferLength);
    int writeURI(const char *protocol, const char *uri, const char *info);
    int readURI(char *s);
    int writeUnabridgedURI(const char *uri, const char *info);
    int readUnabridgedURI(char *s);
    int writeSMS(const char *phoneNumber, const char *message, const char *info);
    int readSMS(char *phoneNumber, char *message);
    int writeGEO(const char *latitude, const char *longitude, const char *info);
    int readGEO(char *latitude, char *longitude);
    int writeEMail(const char *emailAdd, const char *subject, const char *message, const char *info);
    int readEMail(char *emailAdd, char *subject, char *message);
    int writeBluetoothOOB(Ndef_Bluetooth_OOB_t *pBluetooth, char *RecordID);
    NDEF_TypeDef readNDEFType();
    NDEF *getNDEF();

protected:
    NFCTAG_StatusTypeDef ST25DV_Init();
    void ST25DV_GPO_Init();
    void ST25DV_GPO_DeInit();
    uint8_t ST25DV_GPO_ReadPin();
    void ST25DV_LPD_Init();
    void ST25DV_LPD_DeInit();
    uint8_t ST25DV_LPD_ReadPin();
    void ST25DV_LPD_WritePin(uint8_t LpdPinState);
    void ST25DV_I2C_Init();
    void ST25DV_SelectI2cSpeed(uint8_t i2cspeedchoice);

    i2c_config_t i2c_port;
    ST25DV_IO st25dv_io;
    NDEF ndef;
    uint8_t NfctagInitialized = 0;
};

#endif
