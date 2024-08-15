#include "http_server.h"

#include "esp_http_server.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_check.h"

#include "sensors.h"
#include "index_html.h"


#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)

static const char *TAG = "http_server";

static const httpd_uri_t route__root = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = [](httpd_req_t *req) {
            const char *resp_str = (const char *) webapp_dist_index_html;
            httpd_resp_send(req, resp_str, webapp_dist_index_html_len);
            httpd_resp_send_chunk(req, NULL, 0);
            return ESP_OK;
        },
        .user_ctx  = NULL
};

static const httpd_uri_t route_sensors = {
        .uri       = "/sensors.json",
        .method    = (httpd_method_t) HTTP_ANY,
        .handler   = [](httpd_req_t *req) {
            char buf[128];
            sprintf(buf, "{\"temperature\":%d,\"humidity\":%d}", sensors.temperature, sensors.humidity);

            httpd_resp_set_type(req, "application/json");
            httpd_resp_send(req, buf, HTTPD_RESP_USE_STRLEN);
            httpd_resp_send_chunk(req, NULL, 0);
            return ESP_OK;
        },
        .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        static const httpd_uri_t route__favicon = {
                .uri       = "/favicon.ico",
                .method    = (httpd_method_t) HTTP_ANY,
                .handler   = [](httpd_req_t *req) {
                    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not Found!");
                    return ESP_OK;
                },
                .user_ctx  = NULL
        };

        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &route__root);
        httpd_register_uri_handler(server, &route_sensors);
        httpd_register_uri_handler(server, &route__favicon);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}


static esp_err_t stop_webserver(httpd_handle_t server) {
    // Stop the httpd server
    return httpd_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    httpd_handle_t *server = (httpd_handle_t *) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

static void connect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    httpd_handle_t *server = (httpd_handle_t *) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


void vTaskHttpServer(__unused void *pvParams) {
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    /* Start the server for the first time */
    server = start_webserver();

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(5'000));
    }
}
