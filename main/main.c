#include "can.h"
#include "datatypes.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gap.h"
#include "gatt_and_ota.h"
#include "led_strip.h"
#include "nvs_stuff.h"
#include <stdio.h>

#define LED_TSK_PRIORITY 15
#define REMOTE_TSK_PRIORITY 10

static const char *LOG_TAG = "main";

void set_current(int32_t current) {
  current *= 1000;
  uint8_t bytes_send[4];
  bytes_send[3] = current & 0xFF;
  bytes_send[2] = (current >> 8) & 0xFF;
  bytes_send[1] = (current >> 16) & 0xFF;
  bytes_send[0] = (current >> 24) & 0xFF;
  send_command(CAN_PACKET_SET_CURRENT, bytes_send);
}

void remote_task(void *arg) {

  ESP_LOGI(LOG_TAG, "Remote task waiting...");
  vTaskDelay(pdMS_TO_TICKS(5000));
  ESP_LOGI(LOG_TAG, "Sending current commands!");
  for (int i = 0; i < 20; i++) {
    set_current(10);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  ESP_LOGI(LOG_TAG, "Finished!");
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void app_main(void) {
  check_running_partition();
  // can_init();
  setup_nvs();
  ble_setup();

  // xTaskCreate(remote_task, "remote_tsk", 4096, NULL, REMOTE_TSK_PRIORITY,
  // NULL);
  //  xTaskCreate(led_strip_task, "led_tsk", 4096, NULL, LED_TSK_PRIORITY,
  //  NULL);
}
