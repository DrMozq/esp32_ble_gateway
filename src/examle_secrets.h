#pragma once

static const char* GATEWAY_NAME = "esp32";
static const char* AP_PASSWORD = "ypur_ap_pass";

static const char* TARGET_TAGS[] = {
    "00:00:00:00:00:00",
    "11:11:11:11:11:11",
    "22:22:22:22:22:22" 
};

static const int TARGET_TAGS_COUNT = sizeof(TARGET_TAGS) / sizeof(TARGET_TAGS[0]);