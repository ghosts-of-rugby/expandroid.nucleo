#ifndef EXPANDROID_LED_H_
#define EXPANDROID_LED_H_

#include "cJSON/cJSON.h"
#include "main.h"

void control_led_from_json(cJSON *json, GPIO_TypeDef *port, uint16_t pin);

#endif