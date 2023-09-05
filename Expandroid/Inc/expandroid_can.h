#ifndef __EXPANDROID_CAN_H__
#define __EXPANDROID_CAN_H__

#include "cJSON/cJSON.h"
#include "stm32f7xx_hal.h"

void control_can_from_json(cJSON *json, CAN_HandleTypeDef *hcan);
void start_can(CAN_HandleTypeDef *hcan);
cJSON *get_rx_header_and_message_json(CAN_RxHeaderTypeDef *header,
                                      uint8_t *message);
#endif  // __EXPANDROID_CAN_H__