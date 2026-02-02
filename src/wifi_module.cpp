#include "wifi_module.h"
#include "logs.h"
#include "config.h"
#include "secrets.h"
#include <WiFiManager.h>
#include <WiFi.h>


WiFiManager wm;

void initWiFi() 
{
    printLog(INFO, "WiFi: initialization...");

    loadConfig(); // заполняем config из flash

    // Создаем поля для ввода в wm и заполняем их как в config. 
    // Параметры: id, название поля, чем заполняем по умолчанию, длина поля
    WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server *", 
        config.mqtt_server.c_str(), 40);
    WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", 
        config.mqtt_port.c_str(), 6);
    WiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT User", 
        config.mqtt_user.c_str(), 30);
    WiFiManagerParameter custom_mqtt_pass("mqtt_pass", "MQTT Password", 
        config.mqtt_pass.c_str(), 30, "type='password'");

    // Добавляем параметры в wm
    wm.addParameter(&custom_mqtt_server);
    wm.addParameter(&custom_mqtt_port);
    wm.addParameter(&custom_mqtt_user);
    wm.addParameter(&custom_mqtt_pass);

    wm.setTitle("ESP32 Config Portal");
    wm.setConfigPortalTimeout(300); // портал работает 5 минут, после-перезапуск

    // попытка подключения с конфигом из flash, если не получается, то раздается ap
    if (!wm.autoConnect(GATEWAY_NAME, AP_PASSWORD)) {
        // попадем сюда через 5 минут или при вводе неправильного пароля wifi в портале
        printLog(ERROR, "WiFi connection failed!"); 
        delay(3000);
        ESP.restart(); // перезапуск esp32
    }

    // заполнение конфига на основе введенных данных
    config.mqtt_server = custom_mqtt_server.getValue();
    config.mqtt_port   = custom_mqtt_port.getValue();
    config.mqtt_user   = custom_mqtt_user.getValue();
    config.mqtt_pass   = custom_mqtt_pass.getValue();
    saveConfig(); // загружаем config из flash
    
    printLog(SUCCESS, "WiFi connected!");
    printLog(INFO, "SSID: " + WiFi.SSID());
}

bool WiFiconnected() {
    if (WiFi.status() == WL_CONNECTED) {
        return true;
    }
    printLog(ERROR, "WiFi connection failed");
    return false;
}

void resetWiFi() 
{
    printLog(WARNING, "Starting factory reset: clearing WiFi and LittleFS config");
    
    // Сброс WiFiManager (очищает сохранённые Wi-Fi credentials)
    wm.resetSettings();
    printLog(SUCCESS, "WiFiManager settings reset");
    
    // Перезагрузка ESP32
    delay(2000);  // Пауза для логов
    ESP.restart();
}