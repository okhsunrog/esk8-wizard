//
// Created by okhsunrog on 8/19/23.
//

#include "led_strip.h"
#include "esp_log.h"
#include "esp_ws28xx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

CRGB *ws2812_buffer;

static const char *LOG_TAG = "led";

CRGB hsv_to_rgb(uint16_t hue, uint8_t sat, uint8_t val) {
    CRGB res;
    // see Adafruit_NeoPixel lib
    hue = (hue * 1530L + 32768) / 65536;
    // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
    if (hue < 510) { // Red to Green-1
        res.b = 0;
        if (hue < 255) { //   Red to Yellow-1
            res.r = 255;
            res.g = hue;       //     g = 0 to 254
        } else {               //   Yellow to Green-1
            res.r = 510 - hue; //     r = 255 to 1
            res.g = 255;
        }
    } else if (hue < 1020) { // Green to Blue-1
        res.r = 0;
        if (hue < 765) { //   Green to Cyan-1
            res.g = 255;
            res.b = hue - 510;  //     b = 0 to 254
        } else {                //   Cyan to Blue-1
            res.g = 1020 - hue; //     g = 255 to 1
            res.b = 255;
        }
    } else if (hue < 1530) { // Blue to Red-1
        res.g = 0;
        if (hue < 1275) {       //   Blue to Magenta-1
            res.r = hue - 1020; //     r = 0 to 254
            res.b = 255;
        } else { //   Magenta to Red-1
            res.r = 255;
            res.b = 1530 - hue; //     b = 255 to 1
        }
    } else { // Last 0.5 Red (quicker than % operator)
        res.r = 255;
        res.g = res.b = 0;
    }
    // Apply saturation and value to R,G,B, pack into 32-bit result:
    uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
    uint16_t s1 = 1 + sat;  // 1 to 256; same reason
    uint8_t s2 = 255 - sat; // 255 to 0
    res.r = (((res.r * s1) >> 8) + s2) * v1;
    res.g = (((res.g * s1) >> 8) + s2) * v1;
    res.b = (((res.b * s1) >> 8) + s2) * v1;
    return res;
}

void led_strip_task(void *arg) {
    // Init LED strip
    uint freeRAM = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    ESP_LOGI(LOG_TAG, "free RAM is %d.", freeRAM);
    ESP_ERROR_CHECK(
        ws28xx_init(CONFIG_LED_STRIP_GPIO, WS2815, 10, &ws2812_buffer));
    for (;;) {
        for (uint16_t hue = 0; hue < 10000; hue += 4) {
            ws28xx_fill(hsv_to_rgb(hue, 255, 50), 0, 10);
            ESP_ERROR_CHECK(ws28xx_update());
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}
