idf_component_register(
    SRCS 
    "DeviceSetup.cpp"

    INCLUDE_DIRS 
    "include"

    REQUIRES FreeRTOSIntegration bt driver ST25DVSensor
)