#include "hal_gpio_interface.h"

hal_gpio_driver_port_t const *gpio_driver = &HAL_GPIO_DRIVER_PORT;

/* ---------- Generic wrapper macro for all GPIO operations ---------- */
/* This macro generates wrapper functions that check for NULL and call the driver */
#define X(name, return_type, params, ...) \
    return_type hal_gpio_ ## name params \
    { \
        if(gpio_driver->name == NULL) return -1; \
        return gpio_driver->name(__VA_ARGS__); \
    }

/* Generate all wrapper functions from HAL_GPIO_OPERATIONS X-macro */
HAL_GPIO_OPERATIONS

#undef X