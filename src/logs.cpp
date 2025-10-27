#include "logs.h"
#include "mqtt_module.h"
#include "ntp_module.h"
#include "secrets.h"
#include <Arduino.h>


void initLogs() 
{
    Serial.begin(115200);
    while (!Serial); // Ждем Serial для отладки
    delay(3000); // надо подождать, чтобы увидеть первые сообщения
    Serial.println("Serial is initialized");
}

void printLog(LogLevel level, const String& message) 
{
    String timeStamp = getTimeStamp(); 
    
    String levelStr;
    switch (level) {
        case INFO:      levelStr = "INFO"; break;
        case WARNING:   levelStr = "WARNING"; break;
        case SUCCESS:   levelStr = "SUCCESS"; break;
        case ERROR:     levelStr = "ERROR"; break;
    }
    String resMsg = "[" + timeStamp + "] [" + levelStr + "] " + message;

    // пытаемcя опубликовать в mqtt
    String topicLogs = String(GATEWAY_NAME) + "/logs";
    if (publishMQTT(topicLogs, resMsg)) {
        resMsg += " [+MQTT]";
    }

    Serial.println(resMsg);
}