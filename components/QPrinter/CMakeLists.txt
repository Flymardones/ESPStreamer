idf_component_register(
    SRCS 
    "QPrint.cpp"
    "PrintfQPrinter.cpp"

    INCLUDE_DIRS 
    "include"

    REQUIRES FreeRTOSIntegration esp_ringbuf
)


