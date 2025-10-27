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

void pubFoundNRF() 
{
    printLog(INFO, "Starting BLE scan for Nordic (nRF52832) devices...");

    unsigned long scanStart = millis();

    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true); // Активное сканирование для получения доп данных (UUID, RSSI...)
    pBLEScan->setInterval(100); // Интервал между началом двух окон сканирования: 100×0.625=62.5мс
    pBLEScan->setWindow(99); // Длительность активного окна: 61.9 мс (почти всё время активно)

    BLEScanResults foundDevices = pBLEScan->start(10, false);
    int devicesCount = foundDevices.getCount();
    int nordicCount = 0;

    printLog(INFO, "Scan finished. Total devices found: " + String(devicesCount));

    String payload = "{\n  \"devices\": [";

    // найденные устройства
    for (int i = 0; i < devicesCount; i++) {
        BLEAdvertisedDevice device = foundDevices.getDevice(i);
        
        if (device.haveManufacturerData()) {
            std::string mdata = device.getManufacturerData();
            if (mdata.length() >= 2) {
                uint16_t manufacturerID = ((uint8_t)mdata[1] << 8) | (uint8_t)mdata[0]; 

                if (manufacturerID == 0x0059) { // Nordic Semiconductor
                    nordicCount++;
                    String mac = device.getAddress().toString().c_str();
                    String name = device.haveName() ? device.getName().c_str() : "(no name)";
                    int rssi = device.getRSSI();

                    printLog(INFO, "Found Nordic device #" + String(nordicCount) +
                                    " | MAC: " + mac + " | RSSI: " + String(rssi) +
                                    " | Name: " + name);
                    
                    
                    if (nordicCount > 1) payload += ",";
                    payload += "\n    {";
                    payload += "\"mac\":\"" + mac + "\", ";
                    payload += "\"name\":\"" + String(device.getName().c_str()) + "\", ";
                    payload += "\"rssi\":" + String(rssi);
                    payload += "}";
                }
            }
        }
    }
    payload += "\n  ]\n}";

    // MQTT publish
    if (nordicCount > 0) {
        String topicNRF = String(GATEWAY_NAME) + "/nrf_devices";
        if (publishMQTT(topicNRF, payload)) { // попытка опубликовать
            printLog(SUCCESS, "Found nRF devices successfully publish to MQTT ");
        } else {
            printLog(ERROR, "MQTT not connected. Failed publish found nRF devices");
        }
    } else {
        printLog(WARNING, "No nRF devices found in this scan");
    }

    pBLEScan->clearResults();

    unsigned long elapsed = millis() - scanStart;
    printLog(INFO, "Scan + publish cycle done in " + String(elapsed) + " ms");
}