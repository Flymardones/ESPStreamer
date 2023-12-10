idf_component_register(
    SRCS
    "UARTHandler.cpp"

    INCLUDE_DIRS
    "include"

    REQUIRES driver FreeRTOSIntegration NVS bluetooth
    )

component_compile_definitions("FIRMWARE_VERSION=${BUILD_NUM}")
