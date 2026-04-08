/**
 * @file hal_adc_interface.h
 * @author Leonam C S Gomes (leonamcsg@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef HAL_ADC_INTERFACE_H
#define HAL_ADC_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/// estrutura opaca, so carregue com voce e entregue quando for pedido
typedef struct adc_driver_instance_s *adc_handler_t;

/* ---------- X-Macro Definition ---------- */
/* Define all ADC driver operations here for centralized management */
#define HAL_ADC_OPERATIONS \
    X(init, uint8_t, (adc_handler_t *instance), instance) \
    X(deinit, uint8_t, (adc_handler_t instance), instance) \
    X(pin_config, uint8_t, (adc_handler_t instance, uint8_t pin), instance, pin) \
    X(read, uint8_t, (adc_handler_t instance, uint8_t pin, uint16_t *out_value), instance, pin, out_value)

/// Driver functions pointers and typdes definitions
typedef struct hal_adc_driver_port_s
{
#define X(name, return_type, params, ...) return_type (*name) params;
    HAL_ADC_OPERATIONS
#undef X
}hal_adc_driver_port_t;

extern hal_adc_driver_port_t const HAL_ADC_DRIVER_PORT;

/**
 * @brief Inicializa o driver ADC
 * 
 * Retorna -1 em caso driver não referenciado
 * Retorna 0 em caso de sucesso
 * Retorna outro valor diferente de 0 em caso de erro
 * 
 * Recebe o ponteiro para alocar a instancia do driver
 * 
 * @param adc_driver_instance_t* instance
 * @return uint8_t
 */
uint8_t hal_adc_init(adc_handler_t *instance);

/**
 * @brief Configura o pino ADC
 * 
 * @param instance 
 * @param pin 
 * @return true 
 * @return false 
 */
uint8_t hal_adc_pin_config(adc_handler_t instance, uint8_t pin);

/**
 * @brief Le o valor do ADC
 * 
 * @param instance
 * @return bool 
 */
uint8_t hal_adc_read(adc_handler_t instance, uint8_t pin, uint16_t *out_value);

/**
 * @brief Deinicializa o driver ADC
 * 
 * @param instance 
 * @return true 
 * @return false 
 */
uint8_t hal_adc_deinit(adc_handler_t instance);


#ifdef __cplusplus
}
#endif
#endif // HAL_ADC_INTERFACE_H