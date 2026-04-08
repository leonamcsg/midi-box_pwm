/**
 * @file hal_kv_nvs_interface.h
 * @author Leonam C S Gomes (leonamcsg@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef HAL_NVS_INTERFACE_H
#define HAL_NVS_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stddef.h>

/// estrutura opaca, so carregue com voce e entregue quando for pedido
typedef struct nvs_driver_instance_s *nvs_driver_handle_t;

/* ---------- X-Macro Definition ---------- */
/* Define all NVS driver operations here for centralized management */
#define HAL_NVS_OPERATIONS \
    X(init, uint8_t, (nvs_driver_handle_t *instance), instance) \
    X(deinit, uint8_t, (nvs_driver_handle_t instance), instance) \
    X(write_u8, uint8_t, (nvs_driver_handle_t instance, const char* key, uint8_t data), instance, key, data) \
    X(read_u8, uint8_t, (nvs_driver_handle_t instance, const char* key, uint8_t *data), instance, key, data) \
    X(write_str, uint8_t, (nvs_driver_handle_t instance, const char* key, const char* data), instance, key, data) \
    X(read_str, uint8_t, (nvs_driver_handle_t instance, const char *key, char** value), instance, key, value) \
    X(erase, uint8_t, (nvs_driver_handle_t instance, const char* key), instance, key) \
    X(commit, uint8_t, (nvs_driver_handle_t instance), instance)

/// Driver functions pointers and types definitions
typedef struct hal_nvs_driver_port_s
{
#define X(name, return_type, params, ...) return_type (*name) params;
    HAL_NVS_OPERATIONS
#undef X
} hal_nvs_driver_port_t;

extern hal_nvs_driver_port_t const HAL_NVS_DRIVER_PORT;

/**
 * @brief 
 * 
 * @param instance 
 * @return uint8_t 
 */
uint8_t hal_kv_nvs_init(nvs_driver_handle_t *instance);

/**
 * @brief 
 * 
 * @param instance 
 * @return uint8_t 
 */
uint8_t hal_kv_nvs_deinit(nvs_driver_handle_t instance);

/**
 * @brief 
 * 
 * @param instance 
 * @param key 
 * @param value 
 * @return uint8_t 
 */
uint8_t hal_kv_nvs_write_u8(nvs_driver_handle_t instance, const char* key, uint8_t value);

/**
 * @brief 
 * 
 * @param instance 
 * @param key 
 * @param value 
 * @return uint8_t 
 */
uint8_t hal_kv_nvs_read_u8(nvs_driver_handle_t instance, const char* key, uint8_t *value);

/**
 * @brief 
 * 
 * @param instance 
 * @param key 
 * @param value 
 * @return uint8_t 
 */
uint8_t hal_kv_nvs_write_str(nvs_driver_handle_t instance, const char* key, const char* value);

/**
 * @brief 
 * 
 * @param instance 
 * @param key 
 * @param value 
 * @return uint8_t 
 */
uint8_t hal_kv_nvs_read_str(nvs_driver_handle_t instance, const char *key, char** value);

/**
 * @brief 
 * 
 * @param nvs_driver_handle_t instance 
 * @param const char key 
 * @return uint8_t 
 */
uint8_t hal_kv_nvs_erase(nvs_driver_handle_t instance, const char *key);

/**
 * @brief Realiza as operacoes pendentes no NVS
 * 
 * @param instance 
 * @return uint8_t 
 */
uint8_t hal_kv_nvs_commit(nvs_driver_handle_t instance);

#ifdef __cplusplus
}
#endif
#endif // HAL_NVS_INTERFACE_H