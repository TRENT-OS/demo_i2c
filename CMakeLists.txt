#
# LED Demo
#
# Copyright (C) 2019-2020, Hensoldt Cyber GmbH
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
        os_libs
        TimeServer_client
)

DeclareCAmkESComponent(
    I2C
    SOURCES
        components/i2c/I2C.c
    C_FLAGS
        -Wall
        -Werror
    LIBS
        os_core_api
        os_libs
)

#iMX6_I2C_DeclareCAmkESComponent(
#    I2C
#)

TimeServer_DeclareCAmkESComponent(
    TimeServer
)

DeclareAndCreateCamkESSystem(main.camkes)
GenerateSimulateScript()
