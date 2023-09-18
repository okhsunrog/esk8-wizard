//
// Created by okhsunrog on 8/19/23.
//

#include "led_strip.h"
#include "esp_ws28xx.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static uint8_t led_state_off = 0;
CRGB* ws2812_buffer;

static const char *LOG_TAG = "led";


void blink_led(void) {
    for(int i = 0; i < CONFIG_LED_NUM; i++) {
        if (led_state_off) ws2812_buffer[i] = (CRGB){.r=0, .g=0, .b=0};
        else ws2812_buffer[i] = (CRGB){.r=50, .g=0, .b=0};
    }
    ESP_LOGI(LOG_TAG, "updating led strip...");
    ESP_ERROR_CHECK(ws28xx_update());
    ESP_LOGI(LOG_TAG, "updated!");
}

void led_strip_task(void *arg) {
    //Init LED strip
    uint freeRAM = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    ESP_LOGI(LOG_TAG, "free RAM is %d.", freeRAM);
    ESP_ERROR_CHECK(ws28xx_init(CONFIG_LED_STRIP_GPIO, WS2815, 1, &ws2812_buffer));
    for (;;) {
        blink_led();
        led_state_off = !led_state_off;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}