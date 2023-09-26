//
// Created by okhsunrog on 8/20/23.
//

#include "can.h"
#include "esp_log.h"

#define VESC_ID 10

static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
static const twai_filter_config_t f_config = {
    .acceptance_code = 0, .acceptance_mask = 0, .single_filter = true};
static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
    (gpio_num_t) CONFIG_CAN_TX_GPIO, (gpio_num_t) CONFIG_CAN_RX_GPIO, TWAI_MODE_NORMAL);

static const char *LOG_TAG = "can";

void can_init() {
    // Install CAN driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(LOG_TAG, "CAN driver installed");

    // Start CAN driver
    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(LOG_TAG, "CAN driver started");
}

void can_stop() {
    // stop and uninstall twai driver to change hardware filters
    ESP_ERROR_CHECK(twai_stop());
    ESP_ERROR_CHECK(twai_driver_uninstall());
}

void send_command(uint8_t command_id, uint8_t *data4) {
    twai_message_t tx_msg = {};
    tx_msg.extd = 1;
    tx_msg.identifier = VESC_ID + (command_id << 8);
    tx_msg.data_length_code = 4;
    for (int i = 0; i < 4; i++) {
        tx_msg.data[i] = data4[i];
    }
    ESP_ERROR_CHECK(twai_transmit(&tx_msg, portMAX_DELAY));
}
