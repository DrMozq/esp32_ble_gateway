#include "mqtt_module.h"
#include "logs.h"
#include "config.h"
#include "secrets.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>


WiFiClient espClient;
PubSubClient client(espClient); // клиент на основе WiFi для MQTT

bool MQTTconnected() { // проверка подключения MQTT
    return client.connected();
}

void connectMQTT()
{
    client.setServer(config.mqtt_server.c_str(), config.mqtt_port.toInt());
    
    int attempts = 0;
    while (!client.connected() && attempts < 3) { // если еще не подключены, подключаемся
        printLog(INFO, "MQTT: Attempting to connect to " + config.mqtt_server);
        if (client.connect(GATEWAY_NAME, config.mqtt_user.c_str(), config.mqtt_pass.c_str())) {
            printLog(SUCCESS, "MQTT connected to " + config.mqtt_server); 
        } else {
            printLog(ERROR, "MQTT: connection failed to " + config.mqtt_server + 
                ", rc=" + String(client.state()));
            delay(5000); // ждем перед повторной попыткой
        }
        attempts++;
    } 
}

bool publishMQTT(String& topic, String& payload)
{
    if (MQTTconnected()) {
        if (client.publish(topic.c_str(), payload.c_str())) {
            return true;
        }
    }
    return false;
}

void MQTTloop() {
    client.loop(); // обработка входящих сообщений
}