#include "config.h"
#include "logs.h"
#include <LittleFS.h>
#include <ArduinoJson.h>  

Config config;

/*---- Загружает config из flash памяти ----*/
void loadConfig()
{
    if (!LittleFS.begin(true)) { // аргумент true означает, что при неудаче форматируем сиситему 
        printLog(ERROR, "Mouting error LittleFS. LittleFS will be formatted");
        return;
    }
    if (!LittleFS.exists("/config.json")) { // будет создан в saveConfig()
        printLog(WARNING, "File config.json not found. New file will be created");
        return;
    }
    File jsonFile = LittleFS.open("/config.json", "r"); // открываем на чтение
    if (!jsonFile) {
        printLog(ERROR, "Failed to open config.json for reading");
        return;
    }

    JsonDocument dict;
    DeserializationError error = deserializeJson(dict, jsonFile); // config.json -> dictionary
    jsonFile.close();
    LittleFS.end();

    if (error) {
        printLog(ERROR, "Failed to deserialize JSON: " + String(error.c_str()));
        return;
    }  
    // Чтение json и запись в струкуру config
    if (dict["mqtt_server"].is<String>()) config.mqtt_server = dict["mqtt_server"].as<String>();
    if (dict["mqtt_port"].is<String>())   config.mqtt_port = dict["mqtt_port"].as<String>();
    if (dict["mqtt_user"].is<String>())   config.mqtt_user = dict["mqtt_user"].as<String>();
    if (dict["mqtt_pass"].is<String>())   config.mqtt_pass = dict["mqtt_pass"].as<String>();

    printLog(SUCCESS, "Config is loaded from flash");
}

/*---- Загружает config во flash память ----*/
void saveConfig()
{
    if (!LittleFS.begin()) {
        printLog(ERROR, "Mouting error LittleFS to save config");
        return;
    }

    JsonDocument dict; // создаем dict и заполняем
    dict["mqtt_server"] = config.mqtt_server;
    dict["mqtt_port"]   = config.mqtt_port;
    dict["mqtt_user"]   = config.mqtt_user;
    dict["mqtt_pass"]   = config.mqtt_pass;

    File jsonFile = LittleFS.open("/config.json", "w"); // открываем на запись
    if (!jsonFile) {
        printLog(ERROR, "Failed to open config.json for writing");
        return;
    }
    serializeJsonPretty(dict, jsonFile); // dictionary -> config.json
    
    jsonFile.close();
    LittleFS.end();
    printLog(SUCCESS, "Config is saved to flash");
}