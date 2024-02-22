#
# I2C Demo
#
# Copyright (C) 2021-2024, HENSOLDT Cyber GmbH
# 
# SPDX-License-Identifier: GPL-2.0-or-later
#
# For commercial licensing, contact: info.cyber@hensoldt.net
#


cmake_minimum_required(VERSION 3.7.2)

#-------------------------------------------------------------------------------
# use the SDK
find_package("os-sdk" REQUIRED)
os_sdk_set_defaults()
os_sdk_setup(CONFIG_FILE "system_config.h" CONFIG_PROJECT "system_config")

# Set additional include paths.
CAmkESAddCPPInclude("plat/${PLATFORM}")


#-------------------------------------------------------------------------------
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

os_sdk_create_CAmkES_system("main.camkes")
