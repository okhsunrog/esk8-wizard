//
// Created by okhsunrog on 8/20/23.
//

#ifndef MAIN_CAN_H
#define MAIN_CAN_H

#include "driver/twai.h"
#include "sdkconfig.h"

//functions

void can_init();
void can_stop();
void send_command(uint8_t command_id, uint8_t *data4);

#endif //MAIN_CAN_H
