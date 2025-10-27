#pragma once 

#include <Arduino.h>

enum LogLevel {
    INFO = 0,
    SUCCESS = 1,
    WARNING = 2,
    ERROR = 3
};

void initLogs();
void printLog(LogLevel level, const String& message);
