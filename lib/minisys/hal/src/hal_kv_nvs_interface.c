/**
 * @file hal_kv_nvs_interface.c
 * @author Leonam C S Gomes (leonamcsg@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "hal_kv_nvs_interface.h"

hal_nvs_driver_port_t const *nvs_driver = &HAL_NVS_DRIVER_PORT;

/* ---------- Generic wrapper macro for all NVS operations ---------- */
/* This macro generates wrapper functions that check for NULL and call the driver */
#define X(name, return_type, params, ...) \
    return_type hal_kv_nvs_ ## name params \
    { \
        if (nvs_driver->name == NULL) return -1; \
        return nvs_driver->name(__VA_ARGS__); \
    }

/* Generate all wrapper functions from HAL_NVS_OPERATIONS X-macro */
HAL_NVS_OPERATIONS

#undef X
