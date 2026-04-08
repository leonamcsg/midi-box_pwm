#include "hal_adc_interface.h"

hal_adc_driver_port_t const *adc_driver = &HAL_ADC_DRIVER_PORT;

/* ---------- Generic wrapper macro for all ADC operations ---------- */
/* This macro generates wrapper functions that check for NULL and call the driver */
#define X(name, return_type, params, ...) \
    return_type hal_adc_ ## name params \
    { \
        if (adc_driver->name == NULL) return -1; \
        return adc_driver->name(__VA_ARGS__); \
    }

/* Generate all wrapper functions from HAL_ADC_OPERATIONS X-macro */
HAL_ADC_OPERATIONS

#undef X
