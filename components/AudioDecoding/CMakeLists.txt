idf_component_register(
    SRCS
    "StreamDecoder.cpp"

    INCLUDE_DIRS
    "include"

    REQUIRES
    "FreeRTOSIntegration" "QPrinter" "I2S" "libmad"
)

