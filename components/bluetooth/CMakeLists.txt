idf_component_register(
    SRCS 
    "BluetoothHandler.cpp"
    "BluetoothCallback.cpp" 
    "GAPControl.cpp"
    "SPPControl.cpp"
    "A2DPControl.cpp"
    "AVRCControl.cpp"

    INCLUDE_DIRS 
    "include"
    
    REQUIRES "bt" "I2S" "QPrinter" "DeviceControl"
)


