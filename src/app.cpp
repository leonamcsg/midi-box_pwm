#include "Arduino.h"
#include "app.h"
#include "hal_logger_interface.h"
#include "hal_pwm_interface.h"

#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include "ArduinoOTA.h"

#ifndef APSSID
#define APSSID "ESP8266-AP"
#define APPSK "adminadmin"
#endif

unsigned int localPort = 8888;  // local port to listen on
unsigned int otaPort = 8266;    // OTA port

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged\r\n";        // a string to send back

WiFiUDP Udp;

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

static int frequency = 0;
static pwm_handler_t buzzer_pwm = NULL;

void app_setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    
    hal_pwm_config_t pwm_cfg = { .frequency_hz = 1000 };
    if (hal_pwm_init(&buzzer_pwm, pwm_cfg) != 0) {
        HAL_LOGI("APP", "Erro ao inicializar driver PWM!");
    }

    hal_pwm_pin_config_t pin_cfg = {
        .pin = BUZZER_PIN,
        .duty_cycle = 50,
        .active_low = false
    };
    
    if (hal_pwm_configure_pin(buzzer_pwm, pin_cfg) != 0) {
        HAL_LOGI("APP", "Erro ao configurar pino do Buzzer!");
    }

    hal_pwm_stop(buzzer_pwm, BUZZER_PIN);

    // Configura o ESP8266 como ponto de acesso e estacao/////////////
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    Udp.begin(localPort);
    HAL_LOGI("UDP", "Listening on port %d", localPort);

    HAL_LOGI("","AP IP address: %s", WiFi.softAPIP().toString().c_str());
    HAL_LOGI("","AP SSID: %s", ssid);
    // --- Configuração do ArduinoOTA ---
    
    // Porta padrão e 8266
    ArduinoOTA.setPort(otaPort);

    // Nome que aparecerá no PlatformIO/IDE
    ArduinoOTA.setHostname("midi-box-esp8266");
//    ArduinoOTA.setPassword("admin");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) type = "sketch";
        else type = "filesystem"; // SPIFFS/LittleFS

        /// @warning: Para o hardware para nao ficar "apitando" durante o flash
        if (buzzer_pwm != NULL) {
            hal_pwm_stop(buzzer_pwm, BUZZER_PIN);
        }
        HAL_LOGI("OTA", "Iniciando atualização de %s", type.c_str());
    });

    ArduinoOTA.onEnd([]() {
        HAL_LOGI("OTA", "\nAtualização concluída!");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        HAL_LOGD("OTA", "Progresso: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        HAL_LOGI("OTA", "Erro [%u]: ", error);
        if (error == OTA_AUTH_ERROR) HAL_LOGI("OTA", "Falha de Autenticação");
        else if (error == OTA_BEGIN_ERROR) HAL_LOGI("OTA", "Falha no Início");
        else if (error == OTA_CONNECT_ERROR) HAL_LOGI("OTA", "Falha na Conexão");
        else if (error == OTA_RECEIVE_ERROR) HAL_LOGI("OTA", "Falha na Recepção");
        else if (error == OTA_END_ERROR) HAL_LOGI("OTA", "Falha no Fim");
    });

    ArduinoOTA.begin();
    
    HAL_LOGI("OTA", "OTA Pronto!");

    delay(1000);
    HAL_LOGI("", "=== MIDI BOX (HAL PWM MODE) === ");
    HAL_LOGI("", "Digite uma frequência (0-20000 Hz): ");
}

static int last_frequency = -1;
static bool is_buzzer_on = false;
static int packetSize;

void app_loop() {
    ArduinoOTA.handle();

    packetSize = Udp.parsePacket();

    if (packetSize) {
        HAL_LOGI("UDP", "Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                packetSize,
                Udp.remoteIP().toString().c_str(),
                Udp.remotePort(),
                Udp.destinationIP().toString().c_str(),
                Udp.localPort(),
                ESP.getFreeHeap());

        // Le pacote UDP
        int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        packetBuffer[n] = 0;

        HAL_LOGI("UDP", "Contents: %s", packetBuffer);

        // Verifica se todos caracteres são numéricos
        bool isNumber = true;
        for (int i = 0; i < n; i++) {
            if (!isDigit(packetBuffer[i])) {
                isNumber = false;
                break;
            }
        }

        if (isNumber) {
            int newFreq = atoi(packetBuffer);

            if (newFreq >= 0 && newFreq <= 20000) {
                frequency = newFreq;
                HAL_LOGI("UDP", "Nova frequência via UDP: %d Hz", frequency);
            } else {
                HAL_LOGI("UDP", "Valor fora da faixa permitida (0-20000)");
            }
        } else {
            HAL_LOGI("UDP", "Valor inválido recebido via UDP");
        }

        // Resposta UDP
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(ReplyBuffer);
        Udp.endPacket();
    }

    if (frequency != last_frequency) {
        if (frequency > 0) {
            hal_pwm_set_frequency(buzzer_pwm, frequency);
            if (!is_buzzer_on) {
                hal_pwm_start(buzzer_pwm, BUZZER_PIN);
                digitalWrite(LED_BUILTIN, LOW);
                is_buzzer_on = true;
            }
            digitalWrite(LED_BUILTIN, LOW);
        } else {
            hal_pwm_stop(buzzer_pwm, BUZZER_PIN);
            digitalWrite(LED_BUILTIN, HIGH);
            is_buzzer_on = false;
        }
        last_frequency = frequency;
    }
    
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        if (input.length() > 0) {
            bool isNumber = true;
            for (char c : input) {
                if (!isDigit(c)) {
                    isNumber = false;
                    break;
                }
            }
            
            if (!isNumber) {
                HAL_LOGI("", "Erro: Digite apenas números!");
            } else {
                int newFreq = input.toInt();
                
                if (newFreq < 0) {
                    HAL_LOGI("APP", "Erro: frequência negativa!");
                } else if (newFreq > 20000) {
                    frequency = 20000;
                    HAL_LOGI("APP", "Limite: 20kHz. Frequência: %d", frequency);
                } else {
                    frequency = newFreq;
                    HAL_LOGI("APP", "Frequência: %d Hz", frequency);
                }
            }
        }
    }
}