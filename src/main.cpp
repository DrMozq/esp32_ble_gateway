#include "logs.h"
#include "wifi_module.h"
#include "mqtt_module.h"
#include "ntp_module.h"
#include "ble_module.h"

#include <Arduino.h>


int led_wifi_status = 2;

void setup()
{
    pinMode(led_wifi_status, OUTPUT);
    
    initLogs();    
    initWiFi();
    initNTP();
    connectMQTT();
    initBLE();
}

unsigned long lastLoopTime = 0;
const unsigned long loopInterval = 2000;

void loop()
{
    if (millis() - lastLoopTime >= loopInterval) 
    {
        MQTTloop();

        if (WiFiconnected()) {
            digitalWrite(led_wifi_status, HIGH);   
        } else {
            digitalWrite(led_wifi_status, LOW);
            resetWiFi();
        }
        if (!MQTTconnected()) {
            connectMQTT();
            if (!MQTTconnected()) {
                resetWiFi();
            }
        }

        pubFoundNRF();
        lastLoopTime = millis();
    }
}