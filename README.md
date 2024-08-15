# ESP32 + DHT11 over http

Requirements:
* [ESP-IDF](https://idf.espressif.com/)
* esp32c3 - in theory can be any esp32 chip with wifi, supported by idf
* DHT11 sensor
* nodejs - if you want to edit frontend

## Build
```shell
idf.py set-target esp32c3
```

```shell
idf.py menuconfig
```
Configure youre wifi in "Application" section. SSID, Password and auth mode required. Then press S to save config and Q to exit.

```shell
idf.py build flash       
```

## Run
After connecting the ESP32 to Wi-Fi, you can open the web interface using its IP address.
