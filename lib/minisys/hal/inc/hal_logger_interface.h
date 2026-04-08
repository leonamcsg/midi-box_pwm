/**
 * @file hal_logger_interface.h
 * @author Leonam C S Gomes (leonamcsg@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef HAL_LOGGER_INTERFACE_H
#define HAL_LOGGER_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "hal_configs.h"

/* ---------- Enable / Disable ---------- */
#if HAL_LOG_ENABLE_I|HAL_LOG_ENABLE_E|HAL_LOG_ENABLE_W|HAL_LOG_ENABLE_D|HAL_LOG_ENABLE_ERROR_CHECK
#define HAL_LOG_ENABLE 1
#endif

#if !defined(HAL_LOG_ENABLE_I) && \
    !defined(HAL_LOG_ENABLE_E) && \
    !defined(HAL_LOG_ENABLE_W) && \
    !defined(HAL_LOG_ENABLE_D) && \
    !defined(HAL_LOG_ENABLE_ALL) && \
    !defined(HAL_LOG_ENABLE_ERROR_CHECK) && \
    !defined(HAL_LOG_ENABLE)

#define HAL_LOG_ENABLE_ALL 1
#endif


/* ---------- Interface ---------- */
#if HAL_LOG_ENABLE | HAL_LOG_ENABLE_ALL

/* ---------- Log Levels ---------- */
typedef enum {
    HAL_LOG_LEVEL_NONE = 0,
    HAL_LOG_LEVEL_ERROR,
    HAL_LOG_LEVEL_WARN,
    HAL_LOG_LEVEL_INFO,
    HAL_LOG_LEVEL_DEBUG,
    HAL_LOG_LEVE_MAX
} hal_log_level_t;

#define HAL_LOGGER_OPERATIONS \
    X(error_check, uint8_t, (uint8_t hal_error), hal_error) \
    X(log, uint8_t, (hal_log_level_t level, const char *tag, const char *fmt, va_list args), level, tag, fmt, args)

typedef struct {
#define X(name, ret, proto, ...) ret (*name) proto;
    HAL_LOGGER_OPERATIONS
#undef X
} hal_logger_driver_port_t;

extern const hal_logger_driver_port_t HAL_LOGGER_DRIVER_PORT;

uint8_t hal_logger_log(hal_log_level_t level, const char *tag, const char *fmt, ...);
uint8_t hal_logger_error_check(uint8_t hal_error);
/* ---------- Public Macros ---------- */
#if HAL_LOG_ENABLE_E | HAL_LOG_ENABLE_ALL
#define HAL_LOGE(tag, fmt, ...) \
    hal_logger_log(HAL_LOG_LEVEL_ERROR, tag, fmt, ##__VA_ARGS__)
#else
#define HAL_LOGE(tag, fmt, ...)
#endif
#if HAL_LOG_ENABLE_W | HAL_LOG_ENABLE_ALL
#define HAL_LOGW(tag, fmt, ...) \
    hal_logger_log(HAL_LOG_LEVEL_WARN, tag, fmt, ##__VA_ARGS__)
#else
#define HAL_LOGW(tag, fmt, ...)
#endif
#if HAL_LOG_ENABLE_I | HAL_LOG_ENABLE_ALL
#define HAL_LOGI(tag, fmt, ...) \
    hal_logger_log(HAL_LOG_LEVEL_INFO, tag, fmt, ##__VA_ARGS__)
#else
#define HAL_LOGI(tag, fmt, ...)
#endif
#if HAL_LOG_ENABLE_D | HAL_LOG_ENABLE_ALL
#define HAL_LOGD(tag, fmt, ...) \
    hal_logger_log(HAL_LOG_LEVEL_DEBUG, tag, fmt, ##__VA_ARGS__)
#else
#define HAL_LOGD(tag, fmt, ...)
#endif
#if HAL_LOG_ENABLE_ERROR_CHECK | HAL_LOG_ENABLE_ALL
#define HAL_ERROR_CHECK(hal_error) \
    hal_logger_error_check(hal_error)
#else
#define HAL_ERROR_CHECK(hal_error)  (hal_error)
#endif
#else  /* HAL_LOGGER_ENABLE == 0 */
/* ---------- FULLY DISABLED (NOP) ---------- */
#define HAL_LOGE(tag, fmt, ...)
#define HAL_LOGW(tag, fmt, ...)
#define HAL_LOGI(tag, fmt, ...)
#define HAL_LOGD(tag, fmt, ...)

#define HAL_ERROR_CHECK(hal_error)  (hal_error)

/* Interface symbols intentionally NOT defined */

#endif

#ifdef __cplusplus
}
#endif
#endif // HAL_LOGGER_INTERFACE_H