//
// Created by okhsunrog on 8/19/23.
//

#include "led_strip.h"
#include "FastLED.h"
#include "esp_log.h"
#include "esp_ws28xx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define NUM_LEDS 30
static const char *LOG_TAG = "led";

CRGB leds[NUM_LEDS];

CRGBPalette16 pacifica_palette_1 = {0x000507, 0x000409, 0x00030B, 0x00030D,
                                    0x000210, 0x000212, 0x000114, 0x000117,
                                    0x000019, 0x00001C, 0x000026, 0x000031,
                                    0x00003B, 0x000046, 0x14554B, 0x28AA50};
CRGBPalette16 pacifica_palette_2 = {0x000507, 0x000409, 0x00030B, 0x00030D,
                                    0x000210, 0x000212, 0x000114, 0x000117,
                                    0x000019, 0x00001C, 0x000026, 0x000031,
                                    0x00003B, 0x000046, 0x0C5F52, 0x19BE5F};
CRGBPalette16 pacifica_palette_3 = {0x000208, 0x00030E, 0x000514, 0x00061A,
                                    0x000820, 0x000927, 0x000B2D, 0x000C33,
                                    0x000E39, 0x001040, 0x001450, 0x001860,
                                    0x001C70, 0x002080, 0x1040BF, 0x2060FF};
// Add one layer of waves into the led array
void pacifica_one_layer(CRGBPalette16 &p, uint16_t cistart, uint16_t wavescale,
                        uint8_t bri, uint16_t ioff) {
    uint16_t ci = cistart;
    uint16_t waveangle = ioff;
    uint16_t wavescale_half = (wavescale / 2) + 20;
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        waveangle += 250;
        uint16_t s16 = sin16(waveangle) + 32768;
        uint16_t cs = scale16(s16, wavescale_half) + wavescale_half;
        ci += cs;
        uint16_t sindex16 = sin16(ci) + 32768;
        uint8_t sindex8 = scale16(sindex16, 240);
        CRGB c = ColorFromPalette(p, sindex8, bri, LINEARBLEND);
        leds[i] += c;
    }
}

// Add extra 'white' to areas where the four layers of light have lined up
// brightly
void pacifica_add_whitecaps() {
    uint8_t basethreshold = beatsin8(9, 55, 65);
    uint8_t wave = beat8(7);

    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint8_t threshold = scale8(sin8(wave), 20) + basethreshold;
        wave += 7;
        uint8_t l = leds[i].getAverageLight();
        if (l > threshold) {
            uint8_t overage = l - threshold;
            uint8_t overage2 = qadd8(overage, overage);
            leds[i] += CRGB(overage, overage2, qadd8(overage2, overage2));
        }
    }
}

// Deepen the blues and greens
void pacifica_deepen_colors() {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i].blue = scale8(leds[i].blue, 145);
        leds[i].green = scale8(leds[i].green, 200);
        leds[i] |= CRGB(2, 5, 7);
    }
}

void pacifica_loop() {
    // Increment the four "color index start" counters, one for each wave layer.
    // Each is incremented at a different speed, and the speeds vary over time.
    static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
    static uint32_t sLastms = 0;
    uint32_t ms = GET_MILLIS();
    uint32_t deltams = ms - sLastms;
    sLastms = ms;
    uint16_t speedfactor1 = beatsin16(3, 179, 269);
    uint16_t speedfactor2 = beatsin16(4, 179, 269);
    uint32_t deltams1 = (deltams * speedfactor1) / 256;
    uint32_t deltams2 = (deltams * speedfactor2) / 256;
    uint32_t deltams21 = (deltams1 + deltams2) / 2;
    sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
    sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
    sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
    sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

    // Clear out the LED array to a dim background blue-green
    fill_solid(leds, NUM_LEDS, CRGB(2, 6, 10));

    // Render each of four layers, with different scales and speeds, that vary
    // over time
    pacifica_one_layer(pacifica_palette_1, sCIStart1,
                       beatsin16(3, 11 * 256, 14 * 256), beatsin8(10, 70, 130),
                       0 - beat16(301));
    pacifica_one_layer(pacifica_palette_2, sCIStart2,
                       beatsin16(4, 6 * 256, 9 * 256), beatsin8(17, 40, 80),
                       beat16(401));
    pacifica_one_layer(pacifica_palette_3, sCIStart3, 6 * 256,
                       beatsin8(9, 10, 38), 0 - beat16(503));
    pacifica_one_layer(pacifica_palette_3, sCIStart4, 5 * 256,
                       beatsin8(8, 10, 28), beat16(601));

    // Add brighter 'whitecaps' where the waves lines up more
    pacifica_add_whitecaps();

    // Deepen the blues and greens a bit
    pacifica_deepen_colors();
}

void led_strip_task(void *arg) {
    // Init LED strip
    uint freeRAM = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    ESP_LOGI(LOG_TAG, "free RAM is %d.", freeRAM);
    ESP_ERROR_CHECK(ws28xx_init(CONFIG_LED_STRIP_GPIO, WS2815, NUM_LEDS, leds));
    for (;;) {
        pacifica_loop();
        ESP_ERROR_CHECK(ws28xx_update());
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
