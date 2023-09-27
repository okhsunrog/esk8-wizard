#ifndef MAIN_SYNC_H
#define MAIN_SYNC_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

static QueueHandle_t led_control_queue;

#endif // MAIN_SYNC_H
