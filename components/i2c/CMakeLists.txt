#
# iMX6 I2C driver
#
# Copyright (C) 2021, Hensoldt Cyber GmbH
#

cmake_minimum_required(VERSION 3.17)


#-------------------------------------------------------------------------------
# Declare iMX6 I2C Driver CAmkES Component
#
# Parameters:
#
#   <name>
#     required, component instance name
#
#   LIBS <lib_1> <lib_2> ...
#     additional libraries. 
#
function(iMX6_I2C_DeclareCAmkESComponent name)

    cmake_parse_arguments(
        PARSE_ARGV
        1
        iMX6_I2C
        ""
        ""
        "LIBS"
    )

    DeclareCAmkESComponent(
        ${name}
        SOURCES
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/i2c.c
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/imx6_i2c.c
        C_FLAGS
            -Wall
            -Werror
        LIBS
            os_core_api
            os_libs
            ${iMX6_I2C_LIBS}
    )

endfunction()
