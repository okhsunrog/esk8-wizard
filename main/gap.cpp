//
// Created by okhsunrog on 8/19/23.
//

#include "gap.h"
#include "gatt_and_ota.h"
#include "nvs_flash.h"

uint8_t addr_type;

static const char *LOG_TAG = "gap";

int gap_event_handler(struct ble_gap_event *event, void *arg);

void advertise() {
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields adv_data, scan_data;
    int rc;

    memset(&adv_data, 0, sizeof(adv_data));
    memset(&scan_data, 0, sizeof(scan_data));

    // flags: discoverability + BLE only
    adv_data.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    // not sure if needed
    adv_data.appearance = 0;
    adv_data.appearance_is_present = 1;

    // include power levels
    adv_data.tx_pwr_lvl_is_present = 1;
    adv_data.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    // advertise VESC service UUID, needed for VESC Tool
    adv_data.uuids128 = &gatt_svr_svc_vesc_uuid;
    adv_data.num_uuids128 = 1;
    adv_data.uuids128_is_complete = 1;

    // include in scan response to allow for longer names
    scan_data.name = (uint8_t *)device_name;
    scan_data.name_len = strlen(device_name);
    scan_data.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&adv_data);
    if (rc != 0) {
        ESP_LOGE(LOG_TAG, "Error setting advertisement data: rc=%d", rc);
        return;
    }

    rc = ble_gap_adv_rsp_set_fields(&scan_data);
    if (rc != 0) {
        ESP_LOGE(LOG_TAG, "Error setting scan response data: rc=%d", rc);
        return;
    }

    // start advertising
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(addr_type, NULL, BLE_HS_FOREVER, &adv_params,
                           gap_event_handler, NULL);
    if (rc != 0) {
        ESP_LOGE(LOG_TAG, "Error enabling advertisement data: rc=%d", rc);
        return;
    }
}

void reset_cb(int reason) {
    ESP_LOGE(LOG_TAG, "BLE reset: reason = %d", reason);
}

void sync_cb(void) {
    // determine best address type
    ble_hs_id_infer_auto(0, &addr_type);

    // start advertising
    advertise();
}

int gap_event_handler(struct ble_gap_event *event, void *arg) {
    int rc;
    const struct ble_gap_upd_params con_upd_strct = {
        .itvl_max = 8,
        .itvl_min = 6,
        .latency = 0,
        .supervision_timeout = 400,
        .min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN,
        .max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN,
    };
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        // A new connection was established or a connection attempt failed
        rc = ble_gap_update_params(event->connect.conn_handle, &con_upd_strct);
        ESP_LOGW(LOG_TAG, "upd rc: %d", rc);
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(LOG_TAG, "GAP: Disconnect: reason=%d\n",
                 event->disconnect.reason);

        // Connection terminated; resume advertising
        advertise();
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(LOG_TAG, "GAP: adv complete");
        advertise();
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(LOG_TAG, "GAP: Subscribe: conn_handle=%d",
                 event->connect.conn_handle);
        break;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(LOG_TAG, "GAP: MTU update: conn_handle=%d, mtu=%d",
                 event->mtu.conn_handle, event->mtu.value);
        break;
    }

    return 0;
}

void host_task(void *param) {
    // returns only when nimble_port_stop() is executed
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void ble_setup() {
    // BLE Setup

    // initialize BLE controller and nimble stack
    esp_err_t ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to init nimble %d ", ret);
        return;
    }

    // register sync and reset callbacks
    ble_hs_cfg.sync_cb = sync_cb;
    ble_hs_cfg.reset_cb = reset_cb;

    // initialize service table
    gatt_svr_init();

    // set device name and start host task
    ble_svc_gap_device_name_set(device_name);
    ble_att_set_preferred_mtu(520);
    nimble_port_freertos_init(host_task);
}
