/*
 * Sabre Lite (i.MX6) I2C driver
 *
 * Copyright (C) 2021, HENSOLDT Cyber GmbH
 */

#include "OS_Error.h"
#include "lib_debug/Debug.h"
#include "OS_Dataport.h"

//------------------------------------------------------------------------------
/* The I2C chaser light demo made it necessary to change the address handling
 * in the i2c.c file. As we do not want to modify the libplatsupport files, the
 * following two files were extracted from libplatsupport and modified
 * accordingly.
 * Future projects might want to reference the actual I2C driver files in
 * libplatsupport. The corresponding files in libplatsupport can be included
 * with the include commands in the comments below.
 */
#include "i2c.h"    // instead of #include <platsupport/i2c.h>
#include "imx6_i2c.h"   // instead of #include <platsupport/plat/i2c.h>
//------------------------------------------------------------------------------

#include <platsupport/clock.h>
#include <platsupport/plat/clock.h>
#include <platsupport/mux.h>
#include <platsupport/plat/mux.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#include <camkes.h>
#include <camkes/io.h>

// I2C ID used by lib-platsupport must be set
#if !defined(I2C_CONFIG_ID)
#error "I2C_CONFIG_ID missing"
#endif

typedef struct i2c_config {
    enum i2c_id id;
    enum i2c_slave_speed slave_speed;
    enum i2c_slave_address_size addr_size;
} i2c_config_t;

bool                    init_ok = false;
OS_Dataport_t           port_storage = OS_DATAPORT_ASSIGN(i2c_port);
static i2c_bus_t        i2c_bus;
static ps_io_ops_t      io_ops;
i2c_slave_t             slave;
i2c_config_t            config;

//------------------------------------------------------------------------------
void post_init(void)
{
    Debug_LOG_DEBUG("[%s] %s", get_instance_name(), __func__);

    config.id = I2C_CONFIG_ID;
    config.slave_speed = I2C_SLAVE_SPEED_STANDARD;
    config.addr_size = I2C_SLAVE_ADDR_7BIT;

    int error = camkes_io_ops(&io_ops);
    if (error)
    {
        Debug_LOG_ERROR("camkes_io_ops() failed: rslt = %i", error);
        return;
    }

    error = mux_sys_init(&io_ops,NULL,&io_ops.mux_sys);
    if (error)
    {
        Debug_LOG_ERROR("mux_sys_init() failed: rslt = %i", error);
        return;
    }

    error = clock_sys_init(&io_ops,&io_ops.clock_sys);
    if (error)
    {
        Debug_LOG_ERROR("clock_sys_init() failed: rslt = %i", error);
        return;
    }

    error = i2c_init(config.id,&io_ops,&i2c_bus);
    if(error){
        Debug_LOG_ERROR("i2c_init() failed: rslt = %i", error);
        return;
    }
    init_ok = true;

    Debug_LOG_INFO("%s done",__func__);
}

OS_Error_t
__attribute__((__nonnull__))
i2c_rpc_init_slave(
    size_t dev )
{
    Debug_LOG_DEBUG("Slave initialization of device@0x%zx.",dev);
    int ret = i2c_bus.slave_init(&i2c_bus,
                              dev,
                              config.addr_size,
                              config.slave_speed,
                              0,
                              &slave);
    if(ret){
        Debug_LOG_ERROR("i2c slave_init failed, return code: %d",ret);
        return OS_ERROR_GENERIC;
    }
    Debug_LOG_DEBUG("Slave initialization done.");
    return OS_SUCCESS;
}


//------------------------------------------------------------------------------
// This is a CAmkES RPC interface handler. It's guaranteed that "written"
// never points to NULL.
OS_Error_t
__attribute__((__nonnull__))
i2c_rpc_write_data(
    size_t dev,
    size_t reg,
    size_t size,
    size_t* written)
{
    *written = 0;

    Debug_LOG_DEBUG(
        "I2C write: size %zu (0x%zx)",
        size, size);

    if (!init_ok)
    {
        Debug_LOG_ERROR("initialization failed, fail call %s()", __func__);
        return OS_ERROR_INVALID_STATE;
    }

    size_t dataport_size = OS_Dataport_getSize(port_storage);
    if (size > dataport_size)
    {
        // the client did a bogus request, it knows the data port size and
        // never ask for more data
        Debug_LOG_ERROR(
            "size %zu exceeds dataport size %zu",
            size,
            dataport_size );

        return OS_ERROR_INVALID_PARAMETER;
    }

    //write data to slave device
    const void* buffer = OS_Dataport_getBuf(port_storage);

    uint8_t data[2];
    data[0] = reg;
    data[1] = *((uint8_t *)buffer);
    Debug_LOG_INFO("Sending %d (0x%x) to device@0x%zx at register %zu (0x%zx).",
                    data[1],data[1],dev,data[0],data[0]);


    int ret = slave.slave_write(&slave,
                            data,
                            2,
                            false,
                            NULL,
                            NULL);
    if(ret != 2){
        Debug_LOG_ERROR("Writing to i2c slave device failed.");
        return OS_ERROR_GENERIC;
    }

    *written = size;

    return OS_SUCCESS;
}

OS_Error_t
__attribute__((__nonnull__))
i2c_rpc_read_data(
    size_t dev,
    size_t reg,
    size_t  size,
    size_t* read)
{
    *read = 0;

    Debug_LOG_DEBUG(
        "I2C read: size %zu (0x%zx)",
        size, size);

    if (!init_ok)
    {
        Debug_LOG_ERROR("initialization failed, fail call %s()", __func__);
        return OS_ERROR_INVALID_STATE;
    }

    size_t dataport_size = OS_Dataport_getSize(port_storage);
    if (size > dataport_size)
    {
        // the client did a bogus request, it knows the data port size and
        // never ask for more data
        Debug_LOG_ERROR(
            "size %zu exceeds dataport size %zu",
            size,
            dataport_size );

        return OS_ERROR_INVALID_PARAMETER;
    }

    //read data from slave device
    Debug_LOG_INFO("Reading %zu bytes from device %zu (0x%zx) at register %zu (0x%zx).",
                    size,dev,dev,reg,reg);
    int ret = slave.slave_read(&slave,
                                OS_Dataport_getBuf(port_storage),
                                size,
                                false,
                                NULL,
                                NULL);
    if(ret != size){
        Debug_LOG_ERROR("Reading from i2c slave device failed.");
        return OS_ERROR_GENERIC;
    }
    *read = size;

    return OS_SUCCESS;
}
