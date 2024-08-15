#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "dht11.h"
#include "sensors.h"
#include "wifi.h"
#include "http_server.h"


#define DEFAULT_LED_PIN         GPIO_NUM_8
#define DHT11_PIN               GPIO_NUM_10 // ESP32 pin GPIO10 connected to DHT11 sensor

#define TASK_DEFAULT_PRIORITY   10


void vTaskSensors(__unused void *pvParams) {
    DHT11_init(DHT11_PIN);

    vTaskDelay(pdMS_TO_TICKS(1000));

    for (;;) {
        auto dht_result = DHT11_read();
        sensors.temperature = dht_result.temperature;
        sensors.humidity = dht_result.humidity;
        sensors.dht_status = dht_result.status;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTaskStatusLed(__unused void *pvParams) {
    // inverted, on board led
    gpio_reset_pin(DEFAULT_LED_PIN);
    gpio_set_direction(DEFAULT_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(DEFAULT_LED_PIN, GPIO_FLOATING);
    gpio_set_level(DEFAULT_LED_PIN, 1); // 0 = ON, 1 = OFF

    for (;;) {
        switch (wifi_status) {
            case STATUS_CONNECTING:
                gpio_set_level(DEFAULT_LED_PIN, 0); // ON
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(DEFAULT_LED_PIN, 1); // OFF
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(DEFAULT_LED_PIN, 0); // ON
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(DEFAULT_LED_PIN, 1); // OFF
                vTaskDelay(pdMS_TO_TICKS(1000 - (100 * 3)));
                break;
            case STATUS_CONNECTED:
                gpio_set_level(DEFAULT_LED_PIN, 0); // ON
                vTaskDelay(pdMS_TO_TICKS(10000));
                break;
            case STATUS_FAIL:
                gpio_set_level(DEFAULT_LED_PIN, 0); // ON
                vTaskDelay(pdMS_TO_TICKS(200));
                gpio_set_level(DEFAULT_LED_PIN, 1); // OFF
                vTaskDelay(pdMS_TO_TICKS(200));
                break;
            default:
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;
        }
    }
}

extern "C"
void app_main(void) {
    xTaskCreate(vTaskStatusLed, "led", 1024 * 4, NULL, TASK_DEFAULT_PRIORITY, NULL);
    xTaskCreate(vTaskWifi, "wifi", 1024 * 4, NULL, TASK_DEFAULT_PRIORITY, NULL);

    vTaskDelay(pdMS_TO_TICKS(1'000));
    xTaskCreate(vTaskSensors, "sensors", 1024 * 4, NULL, TASK_DEFAULT_PRIORITY, NULL);
    xTaskCreate(vTaskHttpServer, "http", 1024 * 8, NULL, TASK_DEFAULT_PRIORITY, NULL);

    for (;;) {
        // infinite sleep
        vTaskDelay(pdMS_TO_TICKS(60'000));
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
