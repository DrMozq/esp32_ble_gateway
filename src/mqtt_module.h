#pragma once

#include <Arduino.h>

bool MQTTconnected();
void connectMQTT();
bool publishMQTT(String& topic, String& payload);
void MQTTloop();