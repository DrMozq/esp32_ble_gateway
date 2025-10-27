#pragma once

#include <Arduino.h>

struct Config {
  String mqtt_server;
  String mqtt_port = "1883";
  String mqtt_user;
  String mqtt_pass;
};

extern Config config;

void loadConfig();

void saveConfig();