#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Habilida ou desabilida as mensagens de log e erro
 * 
 * @note Ao definir HAL_LOGGER_ENABLE_ALL 0, nao e nescessario incluir os arquivos hal_logger_interface.c nem hal_logger_driver.c na compilacao
 * 
 */
#define HAL_LOG_ENABLE_ALL 0
//#define HAL_LOG_ENABLE_I 1
//#define HAL_LOG_ENABLE_E 1
//#define HAL_LOG_ENABLE_W 1
//#define HAL_LOG_ENABLE_D 1
//#define HAL_LOG_ENABLE_ERROR_CHECK 1

#ifdef __cplusplus
}
#endif
#endif // HAL_CONFIG_H