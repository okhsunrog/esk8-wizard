//
// Created by okhsunrog on 8/19/23.
//

#ifndef MAIN_GAP_H
#define MAIN_GAP_H

#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "host/ble_hs.h"
#include "nimble/ble.h"
#include "host/ble_hs_adv.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

static const char *device_name = "esk8-wizard";

//functions

void ble_setup();

#endif //MAIN_GAP_H
