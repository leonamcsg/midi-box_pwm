/**
 * @file hal_gpio_interface.h
 * @author Leonam C S Gomes (leonamcsg@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef HAL_GPIO_INTERFACE_H
#define HAL_GPIO_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stddef.h>

#define MAX_GPIO_PINS_PER_INSTANCE 10

/* ---------- Nivel ---------- */
typedef enum {
    HAL_GPIO_LOW = 0,
    HAL_GPIO_HIGH
} hal_gpio_level_t;

/* ---------- Pull ---------- */
typedef enum {
    HAL_GPIO_NOPULL = 0,
    HAL_GPIO_PULLUP,
    HAL_GPIO_PULLDOWN
} hal_gpio_pull_t;

/// estrutura opaca, so carregue com voce e entregue quando for pedido
typedef struct gpio_driver_instance_s *gpio_handler_t;

/* ---------- X-Macro Definition ---------- */
/* Define all GPIO driver operations here for centralized management */
#define HAL_GPIO_OPERATIONS \
    X(init, uint8_t, (gpio_handler_t *gpio_handler), gpio_handler) \
    X(deinit, uint8_t, (gpio_handler_t gpio_handler), gpio_handler) \
    X(configure_pin_dout, uint8_t, (gpio_handler_t gpio_handler, uint16_t pin), gpio_handler, pin) \
    X(configure_pin_din, uint8_t, (gpio_handler_t gpio_handler, uint16_t pin, hal_gpio_pull_t pull_mode), gpio_handler, pin, pull_mode) \
    X(release_pin, uint8_t, (gpio_handler_t gpio_handler, uint16_t pin), gpio_handler, pin) \
    X(write_pin, uint8_t, (gpio_handler_t gpio_handler, uint16_t pin, hal_gpio_level_t level), gpio_handler, pin, level) \
    X(read_pin, uint8_t, (gpio_handler_t gpio_handler, uint16_t pin, hal_gpio_level_t *level), gpio_handler, pin, level)

/// Driver functions pointers and types definitions
typedef struct hal_gpio_driver_port_s
{
#define X(name, return_type, params, ...) return_type (*name) params;
    HAL_GPIO_OPERATIONS
#undef X
} hal_gpio_driver_port_t;

extern hal_gpio_driver_port_t const HAL_GPIO_DRIVER_PORT;

/**
 * @brief 
 * 
 * @param gpio_handler 
 * @return uint8_t 
 */
uint8_t hal_gpio_init(gpio_handler_t *gpio_handler);

/**
 * @brief 
 * 
 * @param gpio_handler 
 * @return uint8_t 
 */
uint8_t hal_gpio_deinit(gpio_handler_t gpio_handler);

/**
 * @brief 
 * 
 * @param gpio_handler 
 * @param pin 
 * @param pull_mode 
 * @return uint8_t 
 */
uint8_t hal_gpio_configure_pin_din(gpio_handler_t gpio_handler, uint16_t pin, hal_gpio_pull_t pull_mode);

/**
 * @brief 
 * 
 * @param gpio_handler 
 * @param pin 
 * @return uint8_t 
 */
uint8_t hal_gpio_configure_pin_dout(gpio_handler_t gpio_handler, uint16_t pin);

/**
 * @brief 
 * 
 * @param gpio_handler 
 * @param pin 
 * @return uint8_t 
 */
uint8_t hal_gpio_write_pin(gpio_handler_t gpio_handler, uint16_t pin, hal_gpio_level_t level);

/**
 * @brief 
 * 
 * @param gpio_handler 
 * @param pin 
 * @param level 
 * @return uint8_t 
 */
uint8_t hal_gpio_read_pin(gpio_handler_t gpio_handler, uint16_t pin, hal_gpio_level_t *level);

/**
 * @brief 
 * 
 * @param gpio_handler 
 * @param pin 
 * @return uint8_t 
 */
uint8_t hal_gpio_release_pin(gpio_handler_t gpio_handler, uint16_t pin);

#ifdef __cplusplus
}
#endif
#endif // HAL_GPIO_INTERFACE_H