#ifndef APP_H
#define APP_H

#include <stdint.h>

#define PWM_PIN 5
#define SAMPLING_RATE 44100
#define LED_PIN 2

#define SSID_WiFi "Gomes"
#define PASSWORD "leonam2108"
#define OTA_PORT 8266

void app_loop(void);

void app_setup(void);

#endif /* APP_H */