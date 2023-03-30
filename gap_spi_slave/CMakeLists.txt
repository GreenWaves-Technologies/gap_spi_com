cmake_minimum_required(VERSION 3.16)

###############################################################################
# Panel Control
###############################################################################
set(TARGET_NAME "multi_spi")
set(TARGET_SRCS multi_spi.c)

###############################################################################
# CMake pre initialization
###############################################################################

set(CONFIG_GAP_SDK_HOME $ENV{GAP_SDK_HOME})
include($ENV{GAP_SDK_HOME}/utils/cmake/setup.cmake)

project(${TARGET_NAME} C ASM)
add_executable(${TARGET_NAME} ${TARGET_SRCS})

###############################################################################
# CMake post initialization
###############################################################################
setupos(${TARGET_NAME})
