/**
 * @file hal_pwm_driver.c
 * @author Leonam C S Gomes (leonamcsg@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <Arduino.h>
#include <Ticker.h>
#include <stdlib.h>

extern "C" {
#include "hal_pwm_interface.h"

#define ESP8266_PWM_MAX_DUTY 1023

/// @warning Abaixo disso, o hardware do ESP8266 falha
#define MIN_HW_FREQ 10

typedef struct
{
    uint16_t pin;
    uint8_t duty;
    bool allocated;
    bool active_low;
    bool is_running;
    Ticker sw_ticker;
} pwm_channel_slot_t;

struct pwm_driver_instance_s
{
    uint32_t frequency;
    pwm_channel_slot_t channels[MAX_PWM_CHANNELS_PER_INSTANCE];
};

/// STATIC FUNCTION PROTOTYPES ///
static pwm_channel_slot_t* find_channel_by_pin(pwm_handler_t instance, uint16_t pin);

/// CALLBACKS ///
static void IRAM_ATTR port_pwm_sw_callback(uint16_t pin) {
    digitalWrite(pin, !digitalRead(pin));
}

///////// DRIVER PORT IMPLEMENTATION /////////////////
static uint8_t port_pwm_init(pwm_handler_t *pwm_handler, hal_pwm_config_t cfg)
{
    if ((pwm_handler == NULL) || (cfg.frequency_hz == 0))
        return 1;

    struct pwm_driver_instance_s *instance =
        (struct pwm_driver_instance_s *)calloc(1, sizeof(struct pwm_driver_instance_s));

    if (instance == NULL)
        return 2;

    instance->frequency = cfg.frequency_hz;

    if (instance->frequency >= MIN_HW_FREQ) {
        analogWriteFreq(instance->frequency);
        analogWriteRange(ESP8266_PWM_MAX_DUTY);
    }

    *pwm_handler = instance;
    return 0;
}

static uint8_t port_pwm_deinit(pwm_handler_t pwm_handler)
{
    if (pwm_handler == NULL)
        return 1;

    for (int i = 0; i < MAX_PWM_CHANNELS_PER_INSTANCE; i++)
    {
        if (pwm_handler->channels[i].allocated)
        {
            uint16_t pin = pwm_handler->channels[i].pin;
            uint32_t idle_level = pwm_handler->channels[i].active_low ? ESP8266_PWM_MAX_DUTY : 0;
            
            analogWrite(pin, idle_level);
            pinMode(pin, INPUT);          

            pwm_handler->channels[i].allocated = false;
        }
    }

    free(pwm_handler);

    return 0;
}

static uint8_t port_pwm_configure_pin(pwm_handler_t pwm_handler, hal_pwm_pin_config_t config)
{
    if (pwm_handler == NULL) return 1;
    if (config.duty_cycle > 100) return 2;

    if (find_channel_by_pin(pwm_handler, config.pin) != NULL) return 3;

    pwm_channel_slot_t *slot = NULL;
    for (int i = 0; i < MAX_PWM_CHANNELS_PER_INSTANCE; i++)
    {
        if (!pwm_handler->channels[i].allocated)
        {
            slot = &pwm_handler->channels[i];
            break;
        }
    }

    if (slot == NULL) return 1;

    pinMode(config.pin, OUTPUT);

    slot->allocated = true;
    slot->pin = config.pin;
    slot->duty = config.duty_cycle;
    slot->active_low = config.active_low;

    uint32_t hw_duty = (config.duty_cycle * ESP8266_PWM_MAX_DUTY) / 100;
    if (config.active_low) hw_duty = ESP8266_PWM_MAX_DUTY - hw_duty;
    
    analogWrite(config.pin, hw_duty);

    return 0;
}

static uint8_t port_pwm_set_frequency(pwm_handler_t pwm_handler, uint32_t frequency_hz)
{
    if (pwm_handler == NULL || frequency_hz == 0) return 1;

    pwm_handler->frequency = frequency_hz;
    
    if (frequency_hz >= MIN_HW_FREQ) {
        analogWriteFreq(frequency_hz);
    }
    
    for (int i = 0; i < MAX_PWM_CHANNELS_PER_INSTANCE; i++) {
        if (pwm_handler->channels[i].allocated && pwm_handler->channels[i].is_running) {
            hal_pwm_start(pwm_handler, pwm_handler->channels[i].pin);
        }
    }
    
    return 0;
}

static uint8_t port_pwm_set_duty(pwm_handler_t pwm_handler, uint16_t pin, uint8_t duty_cycle)
{
    if ((pwm_handler == NULL) || (duty_cycle > 100)) return 1;

    pwm_channel_slot_t *slot = find_channel_by_pin(pwm_handler, pin);
    if (slot == NULL) return 2;

    uint32_t hw_duty = (duty_cycle * ESP8266_PWM_MAX_DUTY) / 100;

    if (slot->active_low)
    {
        hw_duty = ESP8266_PWM_MAX_DUTY - hw_duty;
    }

    analogWrite(slot->pin, hw_duty);
    
    slot->duty = duty_cycle;

    return 0;
}

static uint8_t port_pwm_get_duty(pwm_handler_t pwm_handler, uint16_t pin, uint8_t *duty_cycle)
{
    if ((pwm_handler == NULL) || (duty_cycle == NULL)) return 1;

    pwm_channel_slot_t *slot = find_channel_by_pin(pwm_handler, pin);
    if (slot == NULL) return 1;

    *duty_cycle = slot->duty;
    return 0;
}

static uint8_t port_pwm_start(pwm_handler_t pwm_handler, uint16_t pin)
{
    if (pwm_handler == NULL) return 1;
    pwm_channel_slot_t *slot = find_channel_by_pin(pwm_handler, pin);
    if (slot == NULL) return 1;

    slot->is_running = true;

    if (pwm_handler->frequency < MIN_HW_FREQ) {
        analogWrite(pin, slot->active_low ? ESP8266_PWM_MAX_DUTY : 0);
        
        float interval = 0.5f / (float)pwm_handler->frequency;
        slot->sw_ticker.attach(interval, port_pwm_sw_callback, pin);
    } else {
        slot->sw_ticker.detach();
        
        uint32_t hw_duty = (slot->duty * ESP8266_PWM_MAX_DUTY) / 100;
        if (slot->active_low) hw_duty = ESP8266_PWM_MAX_DUTY - hw_duty;
        analogWrite(pin, hw_duty);
    }
        
    return 0;
}

static uint8_t port_pwm_stop(pwm_handler_t pwm_handler, uint16_t pin)
{
    if (pwm_handler == NULL) return 1;
    pwm_channel_slot_t *slot = find_channel_by_pin(pwm_handler, pin);
    if (slot == NULL) return 2;

    slot->is_running = false;
    slot->sw_ticker.detach();

    uint32_t idle_level = slot->active_low ? ESP8266_PWM_MAX_DUTY : 0;
    analogWrite(slot->pin, idle_level);

    return 0;
}

////////// DRIVER INTERFACE LINKING ///////////////
hal_pwm_driver_port_t const HAL_PWM_DRIVER_PORT = {
    .init = port_pwm_init,
    .deinit = port_pwm_deinit,
    .configure_pin = port_pwm_configure_pin,
    .set_duty = port_pwm_set_duty,
    .set_frequency = port_pwm_set_frequency,
    .get_duty = port_pwm_get_duty,
    .start = port_pwm_start,
    .stop = port_pwm_stop
};

///////////// HELPERS FUNCTIONS /////////////////////
static pwm_channel_slot_t* find_channel_by_pin(pwm_handler_t instance, uint16_t pin)
{
    if (instance == NULL) return NULL;

    for (int i = 0; i < MAX_PWM_CHANNELS_PER_INSTANCE; i++)
    {
        if (instance->channels[i].allocated &&
            instance->channels[i].pin == pin)
        {
            return &instance->channels[i];
        }
    }
    return NULL;
}
} // extern "C"