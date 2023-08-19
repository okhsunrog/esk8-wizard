//
// Created by okhsunrog on 8/19/23.
//

#include "led_strip.h"
#include "esp_ws28xx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static uint8_t led_state_off = 0;
CRGB* ws2812_buffer;


void blink_led(void) {
    for(int i = 0; i < LED_NUM; i++) {
        if (led_state_off) ws2812_buffer[i] = (CRGB){.r=0, .g=0, .b=0};
        else ws2812_buffer[i] = (CRGB){.r=50, .g=0, .b=0};
    }
    ESP_ERROR_CHECK_WITHOUT_ABORT(ws28xx_update());
}

_Noreturn void led_strip_task(void *arg) {
    //Init LED strip
    ESP_ERROR_CHECK_WITHOUT_ABORT(ws28xx_init(LED_GPIO, WS2815, LED_NUM, &ws2812_buffer));
    for (;;) {
        blink_led();
        led_state_off = !led_state_off;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}