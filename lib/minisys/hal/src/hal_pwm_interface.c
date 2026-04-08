/**
 * @file hal_pwm_interface.c
 * @author Leonam C S Gomes (leonamcsg@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "hal_pwm_interface.h"

hal_pwm_driver_port_t const *pwm_driver = &HAL_PWM_DRIVER_PORT;

/* ---------- Generic wrapper macro for all PWM operations ---------- */
/* This macro generates wrapper functions that check for NULL and call the driver */
#define X(name, return_type, params, ...) \
    return_type hal_pwm_ ## name params \
    { \
        if(pwm_driver->name == NULL) return -1; \
        return pwm_driver->name(__VA_ARGS__); \
    }

/* Generate all wrapper functions from HAL_PWM_OPERATIONS X-macro */
HAL_PWM_OPERATIONS

#undef X
