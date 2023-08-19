#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs_stuff.h"
#include "gap.h"
#include "gatt_and_ota.h"
#include "led_strip.h"

#define LED_TSK_PRIORITY 10

static const char *LOG_TAG = "main";



void app_main(void) {
    check_running_partition();
    setup_nvs();
    ble_setup();
    xTaskCreate(led_strip_task, "led_tsk", 4096, NULL, LED_TSK_PRIORITY, NULL);

}