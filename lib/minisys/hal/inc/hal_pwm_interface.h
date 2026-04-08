/**
 * @file hal_pwm_interface.h
 * @author Leonam C S Gomes (leonamcsg@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef HAL_PWM_INTERFACE_H
#define HAL_PWM_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_PWM_CHANNELS_PER_INSTANCE 8

/* ---------- PWM Configuration ---------- */
typedef struct {
    uint32_t frequency_hz;
} hal_pwm_config_t;

typedef struct
{
    uint16_t pin;
    uint8_t duty_cycle;
    bool active_low;   // false = ativo alto (default)
} hal_pwm_pin_config_t;

/// estrutura opaca, só carregue com você e entregue quando for pedido
typedef struct pwm_driver_instance_s *pwm_handler_t;

/* ---------- X-Macro Definition ---------- */
/* Define all PWM driver operations here for centralized management */
#define HAL_PWM_OPERATIONS \
    X(init, uint8_t, (pwm_handler_t *pwm_handler, hal_pwm_config_t cfg), pwm_handler, cfg) \
    X(deinit, uint8_t, (pwm_handler_t pwm_handler), pwm_handler) \
    X(configure_pin, uint8_t, (pwm_handler_t pwm_handler, hal_pwm_pin_config_t config), pwm_handler, config) \
    X(set_duty, uint8_t, (pwm_handler_t pwm_handler, uint16_t pin, uint8_t duty_cycle), pwm_handler, pin, duty_cycle) \
    X(set_frequency, uint8_t, (pwm_handler_t pwm_handler, uint32_t frequency_hz), pwm_handler, frequency_hz) \
    X(get_duty, uint8_t, (pwm_handler_t pwm_handler, uint16_t pin, uint8_t *duty_cycle), pwm_handler, pin, duty_cycle) \
    X(start, uint8_t, (pwm_handler_t pwm_handler, uint16_t pin), pwm_handler, pin) \
    X(stop, uint8_t, (pwm_handler_t pwm_handler, uint16_t pin), pwm_handler, pin)

/// Driver functions pointers and types definitions
typedef struct hal_pwm_driver_port_s
{
#define X(name, return_type, params, ...) return_type (*name) params;
    HAL_PWM_OPERATIONS
#undef X
} hal_pwm_driver_port_t;

extern hal_pwm_driver_port_t const HAL_PWM_DRIVER_PORT;

/**
 * @brief Initialize PWM handler
 * 
 * @param pwm_handler 
 * @return uint8_t 
 */
uint8_t hal_pwm_init(pwm_handler_t *pwm_handler, hal_pwm_config_t cfg);

/**
 * @brief Deinitialize PWM handler
 * 
 * @param pwm_handler 
 * @return uint8_t 
 */
uint8_t hal_pwm_deinit(pwm_handler_t pwm_handler);

/**
 * @brief Configure PWM channel
 * 
 * @param pwm_handler 
 * @param config 
 * @return uint8_t 
 */
uint8_t hal_pwm_configure_pin(pwm_handler_t pwm_handler, hal_pwm_pin_config_t config);

/**
 * @brief Set PWM duty cycle
 * 
 * @param pwm_handler 
 * @param pin 
 * @param duty_cycle 
 * @return uint8_t 
 */
uint8_t hal_pwm_set_duty(pwm_handler_t pwm_handler, uint16_t pin, uint8_t duty_cycle);

/**
 * @brief Get PWM duty cycle
 * 
 * @param pwm_handler 
 * @param pin 
 * @param duty_cycle 
 * @return uint8_t 
 */
uint8_t hal_pwm_get_duty(pwm_handler_t pwm_handler, uint16_t pin, uint8_t *duty_cycle);

/**
 * @brief Set PWM frequency
 * 
 * @param pwm_handler 
 * @param frequency_hz 
 * @return uint8_t 
 */
uint8_t hal_pwm_set_frequency(pwm_handler_t pwm_handler, uint32_t frequency_hz);


/**
 * @brief Start PWM on pin
 * 
 * @param pwm_handler 
 * @param pin 
 * @return uint8_t 
 */
uint8_t hal_pwm_start(pwm_handler_t pwm_handler, uint16_t pin);

/**
 * @brief Stop PWM on pin
 * 
 * @param pwm_handler 
 * @param pin 
 * @return uint8_t 
 */
uint8_t hal_pwm_stop(pwm_handler_t pwm_handler, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif // HAL_PWM_INTERFACE_H
