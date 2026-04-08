/**
 * @file hal_logger_interface.c
 * @author Leonam C S Gomes (leonamcsg@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "hal_logger_interface.h"

#if HAL_LOG_ENABLE | HAL_LOG_ENABLE_ALL

static const hal_logger_driver_port_t* logger_driver = &HAL_LOGGER_DRIVER_PORT;

uint8_t hal_logger_error_check(uint8_t hal_error) {
    /* Verifica se o ponteiro e a funcao sao validos */
    if (logger_driver != NULL && logger_driver->error_check != NULL) {
        return logger_driver->error_check(hal_error);
    }
    return -1;
}

uint8_t hal_logger_log(hal_log_level_t level, const char *tag, const char *fmt, ...) {
    uint8_t ret = -1;

    /* Verifica se o ponteiro e a funcao sao validos */
    if (logger_driver != NULL && logger_driver->log != NULL) {
        va_list args;
        va_start(args, fmt);
        
        ret = logger_driver->log(level, tag, fmt, args);
        
        va_end(args);
    }
    return ret;
}

#endif /* HAL_LOGGER_ENABLE */