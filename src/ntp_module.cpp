#include "ntp_module.h"
#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>


// Создаем объект NTPClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600*3, 300000); // UTC+3, обновление каждые 5 минут
// тоже самое, что вызывать timeClient.update() каждый час (в этом коде он вызывается каждые 5 минут)

unsigned long lastNTPUpdate = 0; // последнее время обновления NTP в мс
unsigned long lastLogTime = 0; // последнее время отправления логов в секундах
bool isNTPinit = false;

void initNTP() {
    timeClient.begin(); // Инициализация NTP
    isNTPinit = true;
};

String getTimeStamp() // получить время в формате HH:MM:SS
{
    String timeStamp, timeStampStatus;
    
    if (isNTPinit) { 
        if (timeClient.update()) { // обновляем NTP, если прошло 5 минут с последнего обновления
            lastLogTime = timeClient.getEpochTime(); // время в секундах
            lastNTPUpdate = millis();
            timeStampStatus = "✓";
        } else { // не удалось обновить (не прошло 5 минут или нет подключения к интернету)
            timeStampStatus = "✗";
        }
    }

    // время в секундах (UTC+3 если до этого обновлялось NTP)
    unsigned long epochTime = lastLogTime + (millis() - lastNTPUpdate) / 1000;
    int seconds = epochTime % 60;
    int minutes = (epochTime / 60) % 60;
    int hours = (epochTime / 3600) % 24;

    timeStamp = String(hours / 10) + String(hours % 10) + ':' +
                String(minutes / 10) + String(minutes % 10) + ':' +
                String(seconds / 10) + String(seconds % 10);    
    
    return timeStamp + timeStampStatus;
}