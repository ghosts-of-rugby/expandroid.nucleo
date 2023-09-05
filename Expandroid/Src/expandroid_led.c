#include "expandroid_led.h"

#include <string.h>

void control_led_from_json(cJSON *json, GPIO_TypeDef *port, uint16_t pin) {
  // Control the LED based on the parsed state
  if (cJSON_IsString(json) && json->valuestring != NULL) {
    if (strcmp(json->valuestring, "on") == 0) {
      HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    } else if (strcmp(json->valuestring, "off") == 0) {
      HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    } else if (strcmp(json->valuestring, "toggle") == 0) {
      HAL_GPIO_TogglePin(port, pin);
    }
  }
}
