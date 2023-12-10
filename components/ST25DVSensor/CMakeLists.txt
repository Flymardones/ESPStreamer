idf_component_register(
        SRCS
        "ST25DVSensor.cpp"

        INCLUDE_DIRS
        "include"

        REQUIRES ST25DV_IO libNDEF 
)