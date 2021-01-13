#
# I2C Demo
#
# Copyright (C) 2021, HENSOLDT Cyber GmbH
#

cmake_minimum_required(VERSION 3.7.2)

os_set_config_file(system_config.h)

project(demo_i2c_app C)

set(LibUtilsDefaultZfLogLevel 5 CACHE STRING "" FORCE)

DeclareCAmkESComponent(
    Master
    SOURCES
        components/master/master.c
    LIBS
        os_core_api
        lib_debug
        TimeServer_client
)

include(${CMAKE_CURRENT_SOURCE_DIR}/components/i2c/CMakeLists.txt)
DeclareCAmkESComponents_for_I2Cs()

TimeServer_DeclareCAmkESComponent(
    TimeServer
)

DeclareAndCreateCamkESSystem(main.camkes)
GenerateSimulateScript()
