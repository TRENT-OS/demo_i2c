#
# iMX6 I2C driver
#
# Copyright (C) 2021, HENSOLDT Cyber GmbH
#

cmake_minimum_required(VERSION 3.17)

#-------------------------------------------------------------------------------
#
# Declare I2C driver CAmkES Component
#
# Parameters:
#
#   <name>
#       component name
#   <id>
#       ID used in platsupport libs

#
function(I2C_DeclareCAmkESComponent name id)

    DeclareCAmkESComponent(
        ${name}
        INCLUDES
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/include
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/include/arch/arm
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/include/plat/${PLATFORM}
        SOURCES
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/i2c.c
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/plat/${PLATFORM}/imx6_i2c.c
        C_FLAGS
            -Wall
            -Werror
            -DI2C_CONFIG_ID=${id}
        LIBS
            os_core_api
            lib_debug
            platsupport
    )

endfunction()

#-------------------------------------------------------------------------------
# declare CAmkES components for all I2Cs of a platform
function(DeclareCAmkESComponents_for_I2Cs)

    set(dir_plat "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/plat/${PLATFORM}")

    # For now we assume that all platforms have I2C controllers, so there is always a
    # platform specific folder with CMake files. If it is missing, the build
    # will fail. In case there is ever a platform that has no I2C controllers, there
    # should be an empty platform file that states this fact in a comment.
    if(NOT IS_DIRECTORY "${dir_plat}")
        message(FATAL_ERROR "unsupported platform: '${PLATFORM}'")
    endif()

    CAmkESAddCPPInclude("${dir_plat}")
    include("${dir_plat}/plat_i2c.cmake")

endfunction()
