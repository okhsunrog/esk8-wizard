//
// Created by okhsunrog on 8/19/23.
//

#ifndef MAIN_GATT_AND_OTA_H
#define MAIN_GATT_AND_OTA_H

#include "esp_ota_ops.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define REBOOT_DEEP_SLEEP_TIMEOUT 500

// service: OTA Service
// d6f1d96d-594c-4c53-b1c6-244a1dfde6d8
static const ble_uuid128_t gatt_svr_svc_ota_uuid =
    BLE_UUID128_INIT(0xd8, 0xe6, 0xfd, 0x1d, 0x4a, 024, 0xc6, 0xb1, 0x53, 0x4c,
                     0x4c, 0x59, 0x6d, 0xd9, 0xf1, 0xd6);

// characteristic: OTA Control
// 7ad671aa-21c0-46a4-b722-270e3ae3d830
static const ble_uuid128_t gatt_svr_chr_ota_control_uuid =
    BLE_UUID128_INIT(0x30, 0xd8, 0xe3, 0x3a, 0x0e, 0x27, 0x22, 0xb7, 0xa4, 0x46,
                     0xc0, 0x21, 0xaa, 0x71, 0xd6, 0x7a);

// characteristic: OTA Data
// 23408888-1f40-4cd8-9b89-ca8d45f8a5b0
static const ble_uuid128_t gatt_svr_chr_ota_data_uuid =
    BLE_UUID128_INIT(0xb0, 0xa5, 0xf8, 0x45, 0x8d, 0xca, 0x89, 0x9b, 0xd8, 0x4c,
                     0x40, 0x1f, 0x88, 0x88, 0x40, 0x23);

// service: VESC Service
// 6e400001-b5a3-f393-e0a9-e50e24dcca9e
static const ble_uuid128_t gatt_svr_svc_vesc_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3,
                     0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e);

// characteristic: VESC Characteristic RX
// 6e400002-b5a3-f393-e0a9-e50e24dcca9e
static const ble_uuid128_t gatt_svr_chr_vesc_rx_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3,
                     0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e);

// characteristic: VESC Characteristic TX
// 6e400003-b5a3-f393-e0a9-e50e24dcca9e
static const ble_uuid128_t gatt_svr_chr_vesc_tx_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3,
                     0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e);

// service: Remote Service
// b7708661-d7a3-43f3-98ff-995f8d3d7c63
static const ble_uuid128_t gatt_svr_svc_remote_uuid =
    BLE_UUID128_INIT(0x63, 0x7c, 0x3d, 0x8d, 0x5f, 0x99, 0xff, 0x98, 0xf3, 0x43,
                     0xa3, 0xd7, 0x61, 0x86, 0x70, 0xb7);

// characteristic: LEDs Control Characteristic
// b7708662-d7a3-43f3-98ff-995f8d3d7c63
static const ble_uuid128_t gatt_svr_chr_leds_control_uuid =
    BLE_UUID128_INIT(0x63, 0x7c, 0x3d, 0x8d, 0x5f, 0x99, 0xff, 0x98, 0xf3, 0x43,
                     0xa3, 0xd7, 0x62, 0x86, 0x70, 0xb7);

// functions

void gatt_svr_init();
void check_running_partition();

#endif // MAIN_GATT_AND_OTA_H
