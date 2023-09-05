#include "expandroid_can.h"

#include <string.h>

int parse_tx_header(cJSON *json, CAN_TxHeaderTypeDef *header) {
  cJSON *id_json = cJSON_GetObjectItem(json, "id");
  if (id_json == NULL) {
    return -1;
  }
  uint32_t id = id_json->valueint;
  header->StdId = id;
  header->ExtId = id;

  header->IDE = 0;
  cJSON *is_extended_id_json = cJSON_GetObjectItem(json, "is_extended_id");
  if (is_extended_id_json != NULL) {
    header->IDE = is_extended_id_json->valueint;
  }
  header->RTR = 0;
  cJSON *is_remote_frame_json = cJSON_GetObjectItem(json, "is_remote_frame");
  if (is_remote_frame_json != NULL) {
    header->RTR = is_remote_frame_json->valueint;
  }

  cJSON *dlc_json = cJSON_GetObjectItem(json, "dlc");
  header->DLC = dlc_json->valueint;
  return 0;
}

int parse_tx_message(cJSON *json, uint8_t *message, size_t message_len) {
  // Check if the json is an array
  if (!cJSON_IsArray(json)) {
    return -1;  // Return error if not an array
  }

  // Get the size of the array
  int array_size = cJSON_GetArraySize(json);
  if (array_size > message_len) {
    return -1;  // Return error if the array size is larger than the message
                // buffer
  }

  // Iterate over the array
  for (int i = 0; i < array_size; i++) {
    cJSON *item = cJSON_GetArrayItem(json, i);
    if (!cJSON_IsNumber(item)) {
      return -1;  // Return error if the item is not a number
    }
    message[i] =
        (uint8_t)item->valueint;  // Assign the value to the message buffer
  }

  return 0;  // Return success
}

void start_can(CAN_HandleTypeDef *hcan) {
  CAN_FilterTypeDef filter;
  filter.FilterIdHigh = 0;
  filter.FilterIdLow = 0;
  filter.FilterMaskIdHigh = 0;
  filter.FilterMaskIdLow = 0;
  filter.FilterFIFOAssignment = CAN_RX_FIFO0;
  filter.FilterBank = 0;
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterActivation = ENABLE;
  HAL_CAN_ConfigFilter(hcan, &filter);
  HAL_CAN_Start(hcan);
}

void control_can_from_json(cJSON *json, CAN_HandleTypeDef *hcan) {
  cJSON *action = cJSON_GetObjectItem(json, "action");

  if (cJSON_IsString(action) && action->valuestring != NULL) {
    if (strcmp(action->valuestring, "start") == 0) {
      start_can(hcan);
    } else if (strcmp(action->valuestring, "stop") == 0) {
      HAL_CAN_Stop(hcan);
    } else if (strcmp(action->valuestring, "send") == 0) {
      // Get the CAN header and message
      CAN_TxHeaderTypeDef header;
      uint8_t message[8];
      parse_tx_header(cJSON_GetObjectItem(json, "header"), &header);
      parse_tx_message(cJSON_GetObjectItem(json, "message"), message,
                       sizeof(message));

      // Transmit the CAN message
      uint32_t tx_mailbox;
      HAL_CAN_AddTxMessage(hcan, &header, message, &tx_mailbox);
    }
  }
}

cJSON *get_rx_header_and_message_json(CAN_RxHeaderTypeDef *header,
                                      uint8_t *message) {
  // Create new json objects for the header and message
  cJSON *rx_header_and_message_json = cJSON_CreateObject();

  if (header->IDE == CAN_ID_STD) {
    cJSON_AddNumberToObject(rx_header_and_message_json, "id", header->StdId);
  } else {
    cJSON_AddNumberToObject(rx_header_and_message_json, "id", header->ExtId);
  }
  cJSON_AddNumberToObject(rx_header_and_message_json, "is_extended_id",
                          header->IDE);
  cJSON_AddNumberToObject(rx_header_and_message_json, "is_remote_frame",
                          header->RTR);
  cJSON_AddNumberToObject(rx_header_and_message_json, "dlc", header->DLC);

  cJSON_AddNumberToObject(rx_header_and_message_json, "timestamp",
                          HAL_GetTick());

  cJSON *message_json =
      cJSON_AddArrayToObject(rx_header_and_message_json, "message");

  for (int i = 0; i < header->DLC; i++) {
    cJSON_AddItemToArray(message_json, cJSON_CreateNumber(message[i]));
  }

  return rx_header_and_message_json;
}