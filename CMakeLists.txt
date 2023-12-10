# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# set(EXTRA_CPPFLAGS "FIRMWARE_VERSION=${BUILD_NUM}")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ESPStreamer)


message("Build Number: ${BUILD_NUM}")

