#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "hal_gpio_interface.h"
#include "hal_logger_interface.h"

#define TAG "MIDI_BOX"

gpio_handler_t my_gpio_handler = NULL;

#define PWM_PIN 5
#define SAMPLING_RATE 44100
#define LED_PIN 2

#define SSID "Gomes"
#define PASSWORD "leonam2108"
#define OTA_PORT 8266

// Estrutura da voz atualizada com o Limiar de Duty Cycle
struct Voice {
    uint32_t phase;
    uint32_t phaseIncrement;
    uint32_t dutyThreshold; // NOVO: Controla a largura do pulso (0 a 0xFFFFFFFF)
    bool active;
};

// Inicializa as vozes com 50% de duty cycle (0x80000000) por padrão
volatile Voice voices[2] = {
    {0, 0, 0x80000000, false},
    {0, 0, 0x80000000, false}
};

WiFiUDP udp;
char packetBuffer[255];

static void UI_print_read(void);

////// ISRs ////////////////////////////
void IRAM_ATTR onTimerISR() {
    // NOVO: A comparação agora é feita contra o dutyThreshold (não mais um 50% fixo)
    bool signal1 = (voices[0].active && (voices[0].phase < voices[0].dutyThreshold));
    if (voices[0].active) voices[0].phase += voices[0].phaseIncrement;

    bool signal2 = (voices[1].active && (voices[1].phase < voices[1].dutyThreshold));
    if (voices[1].active) voices[1].phase += voices[1].phaseIncrement;

    if (signal1 ^ signal2) {
        GPOS = (1 << PWM_PIN); // Seta o pino HIGH
    } else {
        GPOC = (1 << PWM_PIN); // Seta o pino LOW
    }

    timer1_write(80000000 / SAMPLING_RATE);
}

void app_setup() {
    Serial.begin(115200);

    if (hal_gpio_init(&my_gpio_handler) != 0) {
        while(1); 
    }

    hal_gpio_configure_pin_dout(my_gpio_handler, PWM_PIN);
    hal_gpio_configure_pin_dout(my_gpio_handler, LED_PIN);
    hal_gpio_write_pin(my_gpio_handler, PWM_PIN, HAL_GPIO_LOW);
    hal_gpio_write_pin(my_gpio_handler, LED_PIN, HAL_GPIO_HIGH);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("MIDI_Box_ESP8266", "adminadmin");
    HAL_LOGI(TAG, "AP Iniciado. IP: %s", WiFi.softAPIP().toString().c_str());

    ArduinoOTA.setPort(OTA_PORT);
    ArduinoOTA.setHostname("midi-box-esp8266");

    HAL_LOGI(TAG, "Conectando a rede: %s...", SSID);
    WiFi.begin(SSID, PASSWORD);

    unsigned long startAttemptTime = millis();
    while (millis() - startAttemptTime < 15000 && WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if(WiFi.status() == WL_CONNECTED) {
        HAL_LOGI(TAG, "Conectado! IP Station: %s", WiFi.localIP().toString().c_str());
    } else {
        HAL_LOGI(TAG, "Falha ao conectar na rede domestica. Operando apenas como AP.");
    }

    udp.begin(8888);

    ArduinoOTA.onStart([]() {
        timer1_disable();
        GPOC = (1 << PWM_PIN);
        HAL_LOGI(TAG, "Iniciando atualizacao OTA...");
    });

    ArduinoOTA.onEnd([]() { HAL_LOGI(TAG, "OTA concluido com sucesso."); });

    ArduinoOTA.onError([](ota_error_t error) {
        HAL_LOGI(TAG, "Erro OTA [%u]", error);
        timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE); 
    });

    ArduinoOTA.begin();

    timer1_attachInterrupt(onTimerISR);
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
    timer1_write(80000000 / SAMPLING_RATE);

    HAL_LOGI(TAG, "Sistema pronto. ISR e HAL ativos.");
}

void app_loop() {
    ArduinoOTA.handle();
    yield(); 
    
    if (udp.parsePacket()) {
        int len = udp.read(packetBuffer, 255);
        if (len > 0) packetBuffer[len] = 0;

        float f; int v; float duty; float s, d; char eff[20];
        // Atualizado para capturar até 6 parâmetros. O sscanf retorna quantos conseguiu ler.
        int parsed = sscanf(packetBuffer, "%f,%d,%f,%f,%f,%s", &f, &v, &duty, &s, &d, eff);
        
        if (parsed >= 2) {
            if (v >= 0 && v < 2) {
                if (f <= 0) {
                    voices[v].active = false;
                    HAL_LOGI(TAG, "Voz %d desligada", v);
                } else {
                    voices[v].phaseIncrement = (uint32_t)((f / SAMPLING_RATE) * 4294967296.0);
                    
                    // Se o usuário enviou o duty cycle (3 ou mais parâmetros)
                    if (parsed >= 3) {
                        if (duty < 0.0) duty = 0.0;
                        if (duty > 100.0) duty = 100.0;
                        voices[v].dutyThreshold = (uint32_t)((duty / 100.0) * 4294967295.0);
                        HAL_LOGI(TAG, "Voz %d -> Freq: %.2f Hz | Duty: %.1f%%", v, f, duty);
                    } else {
                        HAL_LOGI(TAG, "Voz %d -> Freq: %.2f Hz | Duty Mantido", v, f);
                    }

                    voices[v].active = true;
                }
            }
        }
    }

    if (voices[0].active || voices[1].active) {
        hal_gpio_write_pin(my_gpio_handler, LED_PIN, HAL_GPIO_LOW);
    } else {
        hal_gpio_write_pin(my_gpio_handler, LED_PIN, HAL_GPIO_HIGH);
    }

    UI_print_read();
}

///// STATIC FUNCTION DEFINITIONS /////

static void UI_print_read() {
    static bool header_printed = false;

    if (!header_printed) {
        HAL_LOGI("", "\n--- MIDI Box Controller ---\nIP STA (%s):  %s:8888\nIP AP (%s): %s:8888\nComando: freq, voice, [duty, start_time, duration, effect]", 
            SSID,
            WiFi.localIP().toString().c_str(),
            "MIDI_Box_ESP8266",
            WiFi.softAPIP().toString().c_str());
        Serial.print("> ");
        header_printed = true;
    }

    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input.length() > 0) {
            char buffer[255];
            strncpy(buffer, input.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';

            char* command = strtok(buffer, ";");
            
            while (command != NULL) {
                float f; int v; float duty; float s, d; char eff[20];
                
                int parsed = sscanf(command, "%f,%d,%f,%f,%f,%s", &f, &v, &duty, &s, &d, eff);
                if (parsed >= 2) {
                    if (v >= 0 && v < 2) {
                        if (f <= 0) {
                            voices[v].active = false;
                            HAL_LOGI(TAG, "[Serial] Voz %d desligada", v);
                        } else {
                            voices[v].phaseIncrement = (uint32_t)((f / SAMPLING_RATE) * 4294967296.0);
                            
                            if (parsed >= 3) {
                                if (duty < 0.0) duty = 0.0;
                                if (duty > 100.0) duty = 100.0;
                                voices[v].dutyThreshold = (uint32_t)((duty / 100.0) * 4294967295.0);
                                HAL_LOGI(TAG, "[Serial] Voz %d -> Freq: %.2f Hz | Duty: %.1f%%", v, f, duty);
                            } else {
                                HAL_LOGI(TAG, "[Serial] Voz %d -> Freq: %.2f Hz", v, f);
                            }

                            voices[v].active = true;
                        }
                    } else {
                        HAL_LOGI(TAG, "[Serial] Erro: Voz %d invalida. Use 0 ou 1.", v);
                    }
                } else {
                    HAL_LOGI(TAG, "[Serial] Formato invalido: '%s'", command);
                }
                
                command = strtok(NULL, ";");
            }
        }
        Serial.print("> ");
    }
}