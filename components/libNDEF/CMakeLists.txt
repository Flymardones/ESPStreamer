idf_component_register(
        SRCS
        "lib_NDEF_AAR.cpp"
        "lib_NDEF_Bluetooth.cpp"
        "lib_NDEF_Email.cpp"
        "lib_NDEF_Geo.cpp"
        "lib_NDEF_Handover.cpp"
        "lib_NDEF_MyApp.cpp"
        "lib_NDEF_SMS.cpp"
        "lib_NDEF_Text.cpp"
        "lib_NDEF_URI.cpp"
        "lib_NDEF_Vcard.cpp"
        "lib_NDEF_Wifi.cpp"
        "lib_NDEF.cpp"
        "tagtype5_wrapper.cpp"
       

        INCLUDE_DIRS
        "include"

        REQUIRES ST25DV_IO 
)