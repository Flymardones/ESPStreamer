idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS ".")


                    
# target_compile_definitions(SRCS PUBLIC
# FIRMWARE_VERSION=${BUILD_NUM}
# )

component_compile_definitions("FIRMWARE_VERSION=${BUILD_NUM}")



# message("EXTRA: ${EXTRA_CPPFLAGS}")