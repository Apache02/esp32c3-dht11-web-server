#pragma once

typedef struct {
    int temperature;
    int humidity;
    int dht_status;
} sensors_state_t;

extern sensors_state_t sensors;
