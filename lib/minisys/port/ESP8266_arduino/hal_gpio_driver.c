#include "hal_gpio_interface.h"
#include <Arduino.h>

// 1. Definição da estrutura opaca (Opaque Pointer)
// Para este port simples, vamos apenas guardar se o driver foi inicializado
struct gpio_driver_instance_s {
    bool is_initialized;
};

// Instância estática única, já que o ESP8266 não tem múltiplos bancos complexos de GPIO 
// que precisariam de várias instâncias de driver no nosso caso.
static struct gpio_driver_instance_s esp_gpio_instance = { .is_initialized = false };

// 2. Implementação das operações nativas
static uint8_t esp8266_gpio_init(gpio_handler_t *gpio_handler) {
    if (gpio_handler == NULL) return 1; // Erro de ponteiro nulo
    
    esp_gpio_instance.is_initialized = true;
    *gpio_handler = &esp_gpio_instance; // Entrega a instância ao handler
    return 0; // Sucesso
}

static uint8_t esp8266_gpio_deinit(gpio_handler_t gpio_handler) {
    if (gpio_handler == NULL) return 1;
    gpio_handler->is_initialized = false;
    return 0;
}

static uint8_t esp8266_gpio_configure_pin_dout(gpio_handler_t gpio_handler, uint16_t pin) {
    if (gpio_handler == NULL || !gpio_handler->is_initialized) return 1;
    pinMode(pin, OUTPUT);
    return 0;
}

static uint8_t esp8266_gpio_configure_pin_din(gpio_handler_t gpio_handler, uint16_t pin, hal_gpio_pull_t pull_mode) {
    if (gpio_handler == NULL || !gpio_handler->is_initialized) return 1;
    
    uint8_t mode = INPUT;
    if (pull_mode == HAL_GPIO_PULLUP) {
        mode = INPUT_PULLUP;
    }
    // Nota: O ESP8266 nativamente só suporta pull-down interno no GPIO16 com um modo específico.
    // Para fins de abstração genérica, se pedir pulldown, manteremos como INPUT (comportamento padrão).
    
    pinMode(pin, mode);
    return 0;
}

static uint8_t esp8266_gpio_release_pin(gpio_handler_t gpio_handler, uint16_t pin) {
    if (gpio_handler == NULL || !gpio_handler->is_initialized) return 1;
    // Retorna o pino para um estado seguro (entrada flutuante)
    pinMode(pin, INPUT);
    return 0;
}

// ICACHE_RAM_ATTR é essencial aqui porque essa função será chamada pela nossa interrupção (ISR) de áudio!
static uint8_t IRAM_ATTR esp8266_gpio_write_pin(gpio_handler_t gpio_handler, uint16_t pin, hal_gpio_level_t level) {
    if (gpio_handler == NULL || !gpio_handler->is_initialized) return 1;
    digitalWrite(pin, (level == HAL_GPIO_HIGH) ? HIGH : LOW);
    return 0;
}

static uint8_t esp8266_gpio_read_pin(gpio_handler_t gpio_handler, uint16_t pin, hal_gpio_level_t *level) {
    if (gpio_handler == NULL || !gpio_handler->is_initialized || level == NULL) return 1;
    *level = digitalRead(pin) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
    return 0;
}

// 3. O "Vtable" exportado para a interface X-Macro
const hal_gpio_driver_port_t HAL_GPIO_DRIVER_PORT = {
    .init = esp8266_gpio_init,
    .deinit = esp8266_gpio_deinit,
    .configure_pin_dout = esp8266_gpio_configure_pin_dout,
    .configure_pin_din = esp8266_gpio_configure_pin_din,
    .release_pin = esp8266_gpio_release_pin,
    .write_pin = esp8266_gpio_write_pin,
    .read_pin = esp8266_gpio_read_pin
};