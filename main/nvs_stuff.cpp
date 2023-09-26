//
// Created by okhsunrog on 8/17/23.
//

#include "nvs_stuff.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *LOG_TAG = "nvs";

void setup_nvs() {
    ESP_LOGI(LOG_TAG, "initializing NVS");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}
