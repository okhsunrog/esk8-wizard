#ifndef MAIN_SYNC_H
#define MAIN_SYNC_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

extern QueueHandle_t led_control_queue;

#endif // MAIN_SYNC_H
