#include "hal_kv_nvs_interface.h"

////////// DRIVER INTERFACE LINKING ///////////////
hal_nvs_driver_port_t const HAL_NVS_DRIVER_PORT = {
    .init = NULL,
    .deinit = NULL,
    .write_u8 = NULL,
    .read_u8 = NULL,
    .write_str = NULL,
    .read_str = NULL,
    .erase = NULL,
    .commit = NULL
};
