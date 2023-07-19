#include "connections.h"

#include "cJSON/cJSON.h"

int create_udp_server(uint16_t port, int *sock,
                      struct sockaddr_in *client_address,
                      socklen_t *client_address_len) {
  char buffer[128];
  int recv_len;
  struct sockaddr_in address;

  *sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);
  if (*sock < 0) {
    return -1;
  }

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;

  if (lwip_bind(*sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
    lwip_close(*sock);
    return -1;
  }
  *client_address_len = sizeof(*client_address);

  while (1) {
    memset(buffer, 0, sizeof(buffer));
    recv_len =
        lwip_recvfrom(*sock, buffer, sizeof(buffer), 0,
                      ((struct sockaddr *)client_address), client_address_len);
    if (recv_len > 0) {
      if (strcmp(buffer, "hello") == 0) {
        break;
      }
      return 0;
    } else if (recv_len < 0) {
      lwip_close(*sock);
      return -1;
    }
  }

  return 0;
}

void control_led_from_json(cJSON *json) {
  // Get the state of the LEDs
  cJSON *led2 = cJSON_GetObjectItem(json, "led2");
  cJSON *led3 = cJSON_GetObjectItem(json, "led3");

  // Control the LEDs based on the parsed state
  if (cJSON_IsString(led2) && led2->valuestring != NULL) {
    if (strcmp(led2->valuestring, "on") == 0) {
      HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    } else if (strcmp(led2->valuestring, "off") == 0) {
      HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    } else if (strcmp(led2->valuestring, "toggle") == 0) {
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
  }

  if (cJSON_IsString(led3) && led3->valuestring != NULL) {
    if (strcmp(led3->valuestring, "on") == 0) {
      HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
    } else if (strcmp(led3->valuestring, "off") == 0) {
      HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
    } else if (strcmp(led3->valuestring, "toggle") == 0) {
      HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    }
  }
}

int parse_tx_data(cJSON *json, CAN_TxHeaderTypeDef *header, uint8_t *message,
                  size_t message_len) {
  // Ensure the input parameters are valid
  if (!json || !header || !message) {
    return -1;
  }

  // Get the header and message from the JSON
  cJSON *header_json = cJSON_GetObjectItem(json, "header");
  cJSON *message_json = cJSON_GetObjectItem(json, "message");

  // Ensure the header and message are the correct types
  if (!cJSON_IsObject(header_json) || !cJSON_IsArray(message_json)) {
    return -1;
  }

  // Parse the header
  cJSON *stdId_json = cJSON_GetObjectItem(header_json, "StdId");
  cJSON *extId_json = cJSON_GetObjectItem(header_json, "ExtId");
  cJSON *ide_json = cJSON_GetObjectItem(header_json, "IDE");
  cJSON *rtr_json = cJSON_GetObjectItem(header_json, "RTR");
  cJSON *dlc_json = cJSON_GetObjectItem(header_json, "DLC");

  if (cJSON_IsNumber(stdId_json)) {
    header->StdId = stdId_json->valueint;
  }
  if (cJSON_IsNumber(extId_json)) {
    header->ExtId = extId_json->valueint;
  }
  if (cJSON_IsNumber(ide_json)) {
    header->IDE = ide_json->valueint;
  }
  if (cJSON_IsNumber(rtr_json)) {
    header->RTR = rtr_json->valueint;
  }
  if (cJSON_IsNumber(dlc_json)) {
    header->DLC = dlc_json->valueint;
  }

  // Parse the message
  int message_length = cJSON_GetArraySize(message_json);
  for (int i = 0; i < message_length && i < message_len; i++) {
    cJSON *byte_json = cJSON_GetArrayItem(message_json, i);
    if (cJSON_IsNumber(byte_json)) {
      message[i] = byte_json->valueint;
    }
  }

  return 0;
}

void control_can_from_json(cJSON *json, CAN_HandleTypeDef hcan) {
  cJSON *action = cJSON_GetObjectItem(json, "action");

  if (cJSON_IsString(action) && action->valuestring != NULL) {
    if (strcmp(action->valuestring, "start") == 0) {
      HAL_CAN_Start(&hcan);
      // // Get the CAN header and message
      // CAN_TxHeaderTypeDef header;
      // uint8_t message[8];
      // if (parse_tx_data(json, &header, message, sizeof(message)) < 0) {
      //   return;
      // }

      // // Transmit the CAN message
      // uint32_t tx_mailbox;
      // HAL_CAN_AddTxMessage(&hcan, &header, message, &tx_mailbox);
    } else if (strcmp(action->valuestring, "stop") == 0) {
      HAL_CAN_Stop(&hcan);
    } else if (strcmp(action->valuestring, "send") == 0) {
      cJSON *header = cJSON_GetObjectItem(json, "header");
      cJSON *message = cJSON_GetObjectItem(json, "message");
    }
  }

  // // Get the CAN header and message
  // CAN_TxHeaderTypeDef header;
  // uint8_t message[8];
  // if (parse_tx_data(json, &header, message, sizeof(message)) < 0) {
  //   return;
  // }

  // // Transmit the CAN message
  // uint32_t tx_mailbox;
  // HAL_CAN_AddTxMessage(&hcan, &header, message, &tx_mailbox);
}