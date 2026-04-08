/**
 * @file hal_logger_driver.c
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

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "Arduino.h"

/* * Concrete implementation of the Log function.
 * @note Must accept va_list
 */
static uint8_t driver_log(hal_log_level_t level, const char *tag, const char *fmt, va_list args) {
    
    // Add color or formatting based on level
    switch(level) {
        case HAL_LOG_LEVEL_ERROR: printf("\033[0;31mE (%" PRIu64 ") ", micros64()); break; // Red
        case HAL_LOG_LEVEL_WARN:  printf("\033[0;33mW (%" PRIu64 ") ", micros64()); break; // Yellow
        case HAL_LOG_LEVEL_INFO:  printf("\033[0;32mI (%" PRIu64 ") ", micros64()); break; // Green
        case HAL_LOG_LEVEL_DEBUG: printf("\033[0;34mD (%" PRIu64 ") ", micros64()); break; // Blue
        default: printf("\033[0m"); break;
    }

    // Print the Tag
    printf("%s: ", tag);

    // Print the actual formatted message
    vprintf(fmt, args);
    
    // Reset Color and newline
    printf("\033[0m\n\r");
    
    return 0; // Success
}

static uint8_t driver_error_check(uint8_t hal_error) {
    switch (hal_error)
    {
    case 255:
        printf("\033[0;41m[SYSTEM ERROR] Code: %d - [DRIVER FUNCTION NOT LINKED]\033[0m\n", hal_error);
        break;

    case 1:
        printf("\033[0;41m[SYSTEM ERROR] Code: %d - [BAD INSTANCE]\033[0m\n", hal_error);
        break;

    case 0:
        return 0;
        
    default:
        printf("\033[0;41m[SYSTEM ERROR] Code: %d\033[0m\n", hal_error);
        break;
    }

    return hal_error;
}

////////// DRIVER INTERFACE LINKING ///////////////
const hal_logger_driver_port_t HAL_LOGGER_DRIVER_PORT = {
    .log = driver_log,
    .error_check = driver_error_check
};

#endif // HAL_LOGGER_ENABLE
