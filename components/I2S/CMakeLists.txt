idf_component_register(
    SRCS 
    "I2SOutput.cpp"

    INCLUDE_DIRS 
    "include"

    REQUIRES FreeRTOSIntegration QPrinter driver
)


