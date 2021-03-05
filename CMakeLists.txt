#
# I2C Demo
#
# Copyright (C) 2021, HENSOLDT Cyber GmbH
#

cmake_minimum_required(VERSION 3.7.2)

os_set_config_file(
    system_config.h
    CONFIG_PROJECT_NAME system_config)

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
iMX6_I2C_DeclareCAmkESComponent(
    I2C
)

TimeServer_DeclareCAmkESComponent(
    TimeServer
)

DeclareAndCreateCamkESSystem(main.camkes)
GenerateSimulateScript()
