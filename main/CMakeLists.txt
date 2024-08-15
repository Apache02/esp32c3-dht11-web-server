idf_component_register(
        SRCS main.cpp sensors.cpp dht11.cpp wifi.cpp http_server.cpp
        INCLUDE_DIRS ""
        PRIV_REQUIRES esp_common esp_driver_gpio esp_timer esp_wifi nvs_flash esp_http_server
)
