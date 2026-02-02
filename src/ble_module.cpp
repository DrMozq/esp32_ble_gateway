#include "ble_module.h"
#include "logs.h"
#include "mqtt_module.h"
#include "secrets.h"
#include <Arduino.h>
#include <BLEDevice.h>


void initBLE() {
    printLog(INFO, "Initializing BLE...");
    BLEDevice::init(GATEWAY_NAME);
    printLog(SUCCESS, "BLE initialiazed successfully");
}

void pubFoundBLE() 
{
    printLog(INFO, "Starting BLE scan for target devices...");

    unsigned long scanStart = millis();

    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true); // Активное сканирование для получения доп данных (UUID, RSSI...)
    pBLEScan->setInterval(100); // Интервал между началом двух окон сканирования: 100×0.625=62.5мс
    pBLEScan->setWindow(99); // Длительность активного окна: 61.9 мс (почти всё время активно)

    BLEScanResults foundDevices = pBLEScan->start(10, false);
    int devicesCount = foundDevices.getCount();
    int targetCount = 0;

    printLog(INFO, "Scan finished. Total devices found: " + String(devicesCount));

    String payload = "{\n  \"devices\": [";

    // найденные устройства
    for (int i = 0; i < devicesCount; i++) {
        BLEAdvertisedDevice device = foundDevices.getDevice(i);
        String foundMac = device.getAddress().toString().c_str();
        foundMac.toUpperCase(); 

        bool isMyTag = false;
        for (int j = 0; j < TARGET_TAGS_COUNT; j++) {
            if (foundMac == String(TARGET_TAGS[j])) {
                isMyTag = true;
                break;
            }
        }

        if (isMyTag) {
            targetCount++;
            int rssi = device.getRSSI();

            printLog(INFO, "Target found! MAC: " + foundMac + " | RSSI: " + String(rssi));
            
            if (targetCount > 1) payload += ",";
            payload += "\n    {";
            payload += "\"mac\":\"" + foundMac + "\", ";
            payload += "\"rssi\":" + String(rssi);
            payload += "}";
        }
    }
    payload += "\n  ]\n}";

    // MQTT publish
    if (targetCount > 0) {
        String topicNRF = String(GATEWAY_NAME) + "/ble_tags";
        if (publishMQTT(topicNRF, payload)) { // попытка опубликовать
            printLog(SUCCESS, "Found BLE tags successfully publish to MQTT ");
        } else {
            printLog(ERROR, "MQTT not connected. Failed publish found BLE tags");
        }
    } else {
        printLog(WARNING, "No BLE tags found in this scan");
    }

    pBLEScan->clearResults();

    unsigned long elapsed = millis() - scanStart;
    printLog(INFO, "Scan + publish cycle done in " + String(elapsed) + " ms");
}