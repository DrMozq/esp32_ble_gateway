#include "logs.h"
#include "wifi_module.h"
#include "mqtt_module.h"
#include "ntp_module.h"
#include "ble_module.h"

#include <Arduino.h>


const int LED_WIFI_STATUS = 2;
const int RESET_BUTTON_PIN = 4;

void setup()
{
    pinMode(LED_WIFI_STATUS, OUTPUT);
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP); 
    
    initLogs();
    initWiFi();
    initNTP();
    connectMQTT();
    initBLE();
}


unsigned long lastLoopTime = 0;
const unsigned long loopInterval = 2000;
unsigned long buttonPressTime = 0;

void loop()
{
    if (digitalRead(RESET_BUTTON_PIN) == LOW) { 
        if (buttonPressTime == 0) buttonPressTime = millis();
        
        // If you hold down the button for more than 5 seconds
        if (millis() - buttonPressTime > 5000) {
            printLog(WARNING, "Factory Reset triggered by button!");
            resetWiFi();
        }
    } else {
        buttonPressTime = 0;
    }

    // ---- MAIN LOGIC ---- //
    MQTTloop();

    if (millis() - lastLoopTime >= loopInterval) 
    {
        bool wifiOK = WiFiconnected();
        bool mqttOK = MQTTconnected();

        if (wifiOK && mqttOK) {
            digitalWrite(LED_WIFI_STATUS, HIGH);
            pubFoundBLE(); 
        } else {
            // resetWiFi();  // если нет кнопки
            digitalWrite(LED_WIFI_STATUS, LOW);
            if (wifiOK && !mqttOK) connectMQTT();
        }

        lastLoopTime = millis();
    }
}