#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"


void vTaskWifi(void *pvParams);

typedef enum {
    STATUS_INIT = 0,
    STATUS_CONNECTING = 1,
    STATUS_CONNECTED = 2,
    STATUS_FAIL = 3,

} wifi_status_t;

extern wifi_status_t wifi_status;
