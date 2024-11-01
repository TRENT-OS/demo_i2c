/*
 * Master component writes to the port expander and drives the LED chaser
 * light demo.
 *
 * Copyright (C) 2021-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
 */

#include "OS_Error.h"
#include "lib_debug/Debug.h"
#include "OS_Dataport.h"
#include "TimeServer.h"

#include <stdio.h>
#include <string.h>

#include <camkes.h>

#define DEVICE 0x00
#define IODIRA 0x00
#define GPIOA  0x12

static const if_OS_Timer_t timer =
    IF_OS_TIMER_ASSIGN(
        timeServer_rpc,
        timeServer_notify);

OS_Error_t master_main(void)
{
    OS_Error_t err;

    if ((err = i2c_rpc_init_slave(DEVICE)) != OS_SUCCESS)
    {
        Debug_LOG_ERROR("init_slave() failed with %d", err);
        return OS_ERROR_GENERIC;
    }

    OS_Dataport_t port_storage = OS_DATAPORT_ASSIGN(i2c_port);
    uint8_t* buffer = OS_Dataport_getBuf(port_storage);
    memset(buffer,0,OS_Dataport_getSize(port_storage));
    buffer[0] = 0x00;
    size_t bytesWritten = 0;

    // sets all bits of IODIRA-Register to 0
    // => all 8 GPIO ports are defined as outputs
    if ((err = i2c_rpc_write_data(DEVICE,IODIRA,1,&bytesWritten)) != OS_SUCCESS)
    {
        Debug_LOG_ERROR("write_data() to device 0x%x at register 0x%x failed with %d",
                        DEVICE, IODIRA, err);
        return OS_ERROR_GENERIC;
    }

    buffer[0] = 0x00;
    // sets all bits of GPIOA-Register to 0
    // => all 8 GPIO ports are set to 0
    // => all 8 connected LEDs are turned off
    if ((err = i2c_rpc_write_data(DEVICE,GPIOA,1,&bytesWritten)) != OS_SUCCESS)
    {
        Debug_LOG_ERROR("write_data() to device 0x%x at register 0x%x failed with %d",
                        DEVICE, GPIOA, err);
        return OS_ERROR_GENERIC;
    }

    // every loop iteration/every 100 ms, we switch to another LED
    // => chaser light
    while (true)
    {
        unsigned int j = 1;
        for (size_t i = 0; i < 8; i++)
        {
            buffer[0] = j;
            while ((err = i2c_rpc_write_data(DEVICE,GPIOA,1,&bytesWritten)) != OS_SUCCESS)
            {
                Debug_LOG_ERROR("write_data() to register 0x%x of device@0x%x failed with %d",
                                DEVICE, GPIOA, err);
            }
            j *= 2;
            if ((err = TimeServer_sleep(&timer, TimeServer_PRECISION_MSEC,100)) != OS_SUCCESS)
            {
                Debug_LOG_ERROR("TimeServer_sleep() failed with %d", err);
                return OS_ERROR_GENERIC;
            }
        }
    }

    return OS_SUCCESS;
}

int run(void) {
    OS_Error_t ret = master_main();
    if(OS_SUCCESS != ret)
    {
        Debug_LOG_ERROR("master_main failed, code %d",ret);
        return -1;
    }
    return 0;
}
