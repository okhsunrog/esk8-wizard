#include "can.h"
#include "datatypes.h"
#include "gap.h"
#include "gatt_and_ota.h"
#include "led_strip.h"
#include "nvs_stuff.h"
#include "sync.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define LED_TSK_PRIORITY 15
#define REMOTE_TSK_PRIORITY 10

static const char *LOG_TAG = "main";

void remote_task(void *arg) {
    ESP_LOGI(LOG_TAG, "Remote task waiting...");
    vTaskDelay(pdMS_TO_TICKS(5000));
    ESP_LOGI(LOG_TAG, "Sending current commands!");
    for (int i = 0; i < 20; i++) {
        // set_current(15);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    ESP_LOGI(LOG_TAG, "Finished!");
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

extern "C" void app_main(void) {
    // Init sync primitives
    led_control_queue = xQueueCreate(1, sizeof(uint8_t));
    check_running_partition();
    // can_init();
    setup_nvs();
    ble_setup();

    // xTaskCreate(remote_task, "remote_tsk", 4096, NULL, REMOTE_TSK_PRIORITY,
    //             NULL);
    // xTaskCreate(led_strip_task, "led_tsk", 4096, NULL, LED_TSK_PRIORITY, NULL);
    ESP_LOGI(LOG_TAG, "Started!");
}
