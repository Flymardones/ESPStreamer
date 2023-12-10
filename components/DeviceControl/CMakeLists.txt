idf_component_register(
    SRCS 
    "DeviceControl.cpp"

    INCLUDE_DIRS 
    "Include"

    REQUIRES FreeRTOSIntegration bluetooth I2S AudioDecoding QPrinter NVS UARTHandler LEDDriver DeviceSetup
)


