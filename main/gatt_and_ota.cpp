//
// Created by okhsunrog on 8/19/23.
//

#include "gatt_and_ota.h"
#include "datatypes.h"
#include "sync.h"
#include <stdint.h>

uint8_t gatt_svr_chr_ota_control_val;
uint8_t gatt_svr_chr_ota_data_val[520];

uint16_t ota_control_val_handle, ota_data_val_handle, vesc_rx_val_handle,
    vesc_tx_val_handle, leds_control_val_handle;

const esp_partition_t *update_partition;
esp_ota_handle_t update_handle;
bool updating = false;
uint16_t num_pkgs_received = 0;
uint16_t packet_size = 0;

static const char *LOG_TAG = "gatt_and_ota";

static int gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len,
                              uint16_t max_len, void *dst, uint16_t *len);

static int gatt_svr_chr_ota_control_cb(uint16_t conn_handle,
                                       uint16_t attr_handle,
                                       struct ble_gatt_access_ctxt *ctxt,
                                       void *arg);

static int gatt_svr_chr_ota_data_cb(uint16_t conn_handle, uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt *ctxt,
                                    void *arg);

static int gatt_svr_chr_vesc_cb(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

static int gatt_svr_chr_remote_cb(uint16_t conn_handle, uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        // service: VESC Service
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_vesc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    // VESC Characteristic RX
                    .uuid = &gatt_svr_chr_vesc_rx_uuid.u,
                    .access_cb = gatt_svr_chr_vesc_cb,
                    .flags = BLE_GATT_CHR_F_WRITE,
                    .val_handle = &vesc_rx_val_handle,
                },
                {
                    // VESC Characteristic TX
                    .uuid = &gatt_svr_chr_vesc_tx_uuid.u,
                    .access_cb = gatt_svr_chr_vesc_cb,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                    .val_handle = &vesc_tx_val_handle,
                },
                {
                    0, /* No more characteristics in this service */
                }},
    },
    {
        // service: Remote Service
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_remote_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    // LEDs Control Characteristic
                    .uuid = &gatt_svr_chr_leds_control_uuid.u,
                    .access_cb = gatt_svr_chr_remote_cb,
                    .flags = BLE_GATT_CHR_F_WRITE,
                    .val_handle = &leds_control_val_handle,
                },
                {
                    0, /* No more characteristics in this service */
                }},
    },
    {
        // service: OTA Service
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_ota_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    // characteristic: OTA control
                    .uuid = &gatt_svr_chr_ota_control_uuid.u,
                    .access_cb = gatt_svr_chr_ota_control_cb,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE |
                             BLE_GATT_CHR_F_NOTIFY,
                    .val_handle = &ota_control_val_handle,
                },
                {
                    // characteristic: OTA data
                    .uuid = &gatt_svr_chr_ota_data_uuid.u,
                    .access_cb = gatt_svr_chr_ota_data_cb,
                    .flags = BLE_GATT_CHR_F_WRITE,
                    .val_handle = &ota_data_val_handle,
                },
                {
                    0, /* No more characteristics in this service */
                }},
    },
    {
        0, /* No more services */
    },
};

static int gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len,
                              uint16_t max_len, void *dst, uint16_t *len) {
    uint16_t om_len;
    int rc;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

static void update_ota_control(uint16_t conn_handle) {
    struct os_mbuf *om;
    esp_err_t err;

    // check which value has been received
    switch (gatt_svr_chr_ota_control_val) {
    case SVR_CHR_OTA_CONTROL_REQUEST:
        // OTA request
        ESP_LOGI(LOG_TAG, "OTA has been requested via BLE.");
        // get the next free OTA partition
        update_partition = esp_ota_get_next_update_partition(NULL);
        // start the ota update
        err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES,
                            &update_handle);
        if (err != ESP_OK) {
            ESP_LOGE(LOG_TAG, "esp_ota_begin failed (%s)",
                     esp_err_to_name(err));
            esp_ota_abort(update_handle);
            gatt_svr_chr_ota_control_val = SVR_CHR_OTA_CONTROL_REQUEST_NAK;
        } else {
            gatt_svr_chr_ota_control_val = SVR_CHR_OTA_CONTROL_REQUEST_ACK;
            updating = true;

            // retrieve the packet size from OTA data
            packet_size = (gatt_svr_chr_ota_data_val[1] << 8) +
                          gatt_svr_chr_ota_data_val[0];
            ESP_LOGI(LOG_TAG, "Packet size is: %d", packet_size);

            num_pkgs_received = 0;
        }

        // notify the client via BLE that the OTA has been acknowledged (or not)
        om = ble_hs_mbuf_from_flat(&gatt_svr_chr_ota_control_val,
                                   sizeof(gatt_svr_chr_ota_control_val));
        ble_gatts_notify_custom(conn_handle, ota_control_val_handle, om);
        ESP_LOGI(LOG_TAG, "OTA request acknowledgement has been sent.");

        break;

    case SVR_CHR_OTA_CONTROL_DONE:

        updating = false;

        // end the OTA and start validation
        err = esp_ota_end(update_handle);
        if (err != ESP_OK) {
            if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
                ESP_LOGE(LOG_TAG,
                         "Image validation failed, image is corrupted!");
            } else {
                ESP_LOGE(LOG_TAG, "esp_ota_end failed (%s)!",
                         esp_err_to_name(err));
            }
        } else {
            // select the new partition for the next boot
            err = esp_ota_set_boot_partition(update_partition);
            if (err != ESP_OK) {
                ESP_LOGE(LOG_TAG, "esp_ota_set_boot_partition failed (%s)!",
                         esp_err_to_name(err));
            }
        }

        // set the control value
        if (err != ESP_OK) {
            gatt_svr_chr_ota_control_val = SVR_CHR_OTA_CONTROL_DONE_NAK;
        } else {
            gatt_svr_chr_ota_control_val = SVR_CHR_OTA_CONTROL_DONE_ACK;
        }

        // notify the client via BLE that DONE has been acknowledged
        om = ble_hs_mbuf_from_flat(&gatt_svr_chr_ota_control_val,
                                   sizeof(gatt_svr_chr_ota_control_val));
        ble_gatts_notify_custom(conn_handle, ota_control_val_handle, om);
        ESP_LOGI(LOG_TAG, "OTA DONE acknowledgement has been sent.");

        // restart the ESP to finish the OTA
        if (err == ESP_OK) {
            ESP_LOGI(LOG_TAG, "Preparing to restart!");
            vTaskDelay(pdMS_TO_TICKS(REBOOT_DEEP_SLEEP_TIMEOUT));
            esp_restart();
        }

        break;

    default:
        break;
    }
}

static int gatt_svr_chr_ota_control_cb(uint16_t conn_handle,
                                       uint16_t attr_handle,
                                       struct ble_gatt_access_ctxt *ctxt,
                                       void *arg) {
    int rc;
    uint8_t length = sizeof(gatt_svr_chr_ota_control_val);

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        // a client is reading the current value of ota control
        rc = os_mbuf_append(ctxt->om, &gatt_svr_chr_ota_control_val, length);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        break;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        // a client is writing a value to ota control
        rc = gatt_svr_chr_write(ctxt->om, 1, length,
                                &gatt_svr_chr_ota_control_val, NULL);
        // update the OTA state with the new value
        update_ota_control(conn_handle);
        return rc;
        break;

    default:
        break;
    }

    // this shouldn't happen
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int gatt_svr_chr_ota_data_cb(uint16_t conn_handle, uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt *ctxt,
                                    void *arg) {
    int rc;
    esp_err_t err;

    // store the received data into gatt_svr_chr_ota_data_val
    rc = gatt_svr_chr_write(ctxt->om, 1, sizeof(gatt_svr_chr_ota_data_val),
                            gatt_svr_chr_ota_data_val, NULL);

    // write the received packet to the partition
    if (updating) {
        err =
            esp_ota_write(update_handle,
                          (const void *)gatt_svr_chr_ota_data_val, packet_size);
        if (err != ESP_OK) {
            ESP_LOGE(LOG_TAG, "esp_ota_write failed (%s)!",
                     esp_err_to_name(err));
        }

        num_pkgs_received++;
        // ESP_LOGI(LOG_TAG_GATT_SVR, "Received packet %d", num_pkgs_received);
    }

    return rc;
}

static int gatt_svr_chr_vesc_cb(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg) {
    int rc = 0;
    return rc;
}

static int gatt_svr_chr_remote_cb(uint16_t conn_handle, uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt,
                                  void *arg) {
    uint8_t led_control_value;
    int rc;
    rc = gatt_svr_chr_write(ctxt->om, 1, sizeof(led_control_value),
                            &led_control_value, NULL);
    ESP_LOGI(LOG_TAG, "Received some led data: %d", led_control_value);
    xQueueOverwrite(led_control_queue, (void *)&led_control_value);
    return rc;
}

void gatt_svr_init() {
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_gatts_count_cfg(gatt_svr_svcs);
    ble_gatts_add_svcs(gatt_svr_svcs);
}

void check_running_partition() {
    const esp_partition_t *partition = esp_ota_get_running_partition();
    ESP_LOGI(LOG_TAG, "the partition address is: %" PRIu32, partition->address);
    switch (partition->address) {
    case 0x00020000:
        ESP_LOGI(LOG_TAG, "Running partition: ota_0");
        break;
    case 0x00210000:
        ESP_LOGI(LOG_TAG, "Running partition: ota_1");
        break;
    default:
        ESP_LOGE(LOG_TAG, "Running partition: unknown");
        break;
    }
}
