//
// Created by okhsunrog on 8/19/23.
//

#include "led_strip.h"
#include "FastLED.h"
#include "esp_log.h"
#include "esp_ws28xx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

PxRGB *ws2812_buffer;

static const char *LOG_TAG = "led";

void led_strip_task(void *arg) {
    // Init LED strip
    CRGB crgb_tmp;
    PxRGB pxrgb_tmp;
    uint freeRAM = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    ESP_LOGI(LOG_TAG, "free RAM is %d.", freeRAM);
    ESP_ERROR_CHECK(
        ws28xx_init(CONFIG_LED_STRIP_GPIO, WS2815, 10, &ws2812_buffer));
    for (;;) {
        static uint8_t hue;
        hue = hue + 1;
        // Use FastLED automatic HSV->RGB conversion
        crgb_tmp = CHSV(hue, 255, 255);
        pxrgb_tmp = {};
        pxrgb_tmp.r = crgb_tmp.r;
        pxrgb_tmp.g = crgb_tmp.g;
        pxrgb_tmp.b = crgb_tmp.b;
        ws28xx_fill(pxrgb_tmp, 0, 10);
        ESP_ERROR_CHECK(ws28xx_update());
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
