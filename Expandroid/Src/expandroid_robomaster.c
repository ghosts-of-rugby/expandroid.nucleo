#include "expandroid_robomaster.h"

#include <stdio.h>
#include <stm32f7xx_hal_can.h>
#include <string.h>

robomaster_motor motors_can1[MAX_MOTOR_NUM_PER_CANBUS];

void init_motors() {
  // initialize motors
  motors_can1[0].initialized = 0;
  motors_can1[0].ref_current = 0;
  motors_can1[0].ref_speed_rpm = 0;
  motors_can1[0].ref_total_angle = 0;
  motors_can1[0].kp_spd = 3.0;
  motors_can1[0].kp_ang = 0.04;
  motors_can1[0].kd_ang = 4.0;
  motors_can1[0].kp_ang_max_current = 4000;
  motors_can1[0].kd_ang_max_current = 3000;

  motors_can1[1].initialized = 0;
  motors_can1[1].ref_current = 0;
  motors_can1[1].ref_speed_rpm = 0;
  motors_can1[1].ref_total_angle = 0;
  motors_can1[1].kp_spd = 13.0;
  motors_can1[1].kp_ang = 0.3;
  motors_can1[1].kd_ang = 8.0;
  motors_can1[1].kp_ang_max_current = (int)(15 * 16384.0 / 20.0);
  motors_can1[1].kd_ang_max_current = (int)(15 * 16384.0 / 20.0);
}

void process_motor_command(cJSON *json) {
  cJSON *data = NULL;
  cJSON_ArrayForEach(data, json) {
    // get "id"
    cJSON *id = cJSON_GetObjectItem(data, "id");
    // get "mode"
    cJSON *name = cJSON_GetObjectItem(data, "name");
    // get "value"
    cJSON *value = cJSON_GetObjectItem(data, "value");

    // get motor
    robomaster_motor *motor = &motors_can1[id->valueint - 1];

    if (strcmp(name->valuestring, "ref_state") == 0) {
      motor->ctrl_mode = CTRL_STATE;
      motor->ref_total_angle = cJSON_GetArrayItem(value, 0)->valueint;
      motor->ref_speed_rpm = cJSON_GetArrayItem(value, 1)->valueint;
    } else if (strcmp(name->valuestring, "ref_cur") == 0) {
      motor->ctrl_mode = CTRL_CURRENT;
      motor->ref_current = value->valueint;
    } else if (strcmp(name->valuestring, "ref_spd") == 0) {
      motor->ctrl_mode = CTRL_VELOCITY;
      motor->ref_speed_rpm = value->valueint;
    } else if (strcmp(name->valuestring, "ref_ang") == 0) {
      motor->ctrl_mode = CTRL_POSITION;
      motor->ref_total_angle = value->valueint;
    } else if (strcmp(name->valuestring, "kp_spd") == 0) {
      motor->kp_spd = value->valuedouble;
    } else if (strcmp(name->valuestring, "kp_ang") == 0) {
      motor->kp_ang = value->valuedouble;
    } else if (strcmp(name->valuestring, "kd_ang") == 0) {
      motor->kd_ang = value->valuedouble;
    } else if (strcmp(name->valuestring, "kp_ang_max_cur") == 0) {
      motor->kp_ang_max_current = value->valueint;
    } else if (strcmp(name->valuestring, "kd_ang_max_cur") == 0) {
      motor->kd_ang_max_current = value->valueint;
    }
  }
}

void parse_motor_rx_message(CAN_RxHeaderTypeDef *header, uint8_t *message) {
  // parse message
  uint8_t id = header->StdId - 512;

  // get motor
  robomaster_motor *motor = &motors_can1[id - 1];

  if (motor->initialized == 0) {
    motor->initialized = 1;
    motor->cur_angle = (uint16_t)(message[0] << 8 | message[1]);
    motor->prev_angle = (uint16_t)(message[0] << 8 | message[1]);
    motor->offset_angle = (uint16_t)(message[0] << 8 | message[1]);
  }

  // parse data
  motor->prev_angle = motor->cur_angle;
  motor->cur_angle = (uint16_t)(message[0] << 8 | message[1]);
  motor->cur_speed_rpm = (int16_t)(message[2] << 8 | message[3]);
  motor->cur_current = (int16_t)(message[4] << 8 | message[5]);

  if (motor->cur_angle - motor->prev_angle > 4096) {
    motor->round_cnt--;
  } else if (motor->cur_angle - motor->prev_angle < -4096) {
    motor->round_cnt++;
  }
  motor->cur_total_angle =
      motor->round_cnt * 8192 + motor->cur_angle - motor->offset_angle;
}

int clip_value(int value, int min, int max) {
  if (value > max) {
    return max;
  } else if (value < min) {
    return min;
  } else {
    return value;
  }
}

void get_motor_tx_message(CAN_TxHeaderTypeDef *header, uint8_t *message) {
  header->StdId = ROBOMASTER_TX_ID;
  header->IDE = CAN_ID_STD;
  header->RTR = CAN_RTR_DATA;
  header->DLC = 8;

  for (int id = 1; id <= 4; id++) {
    robomaster_motor *motor = &motors_can1[id - 1];
    int16_t input_current = 0;
    if (motor->ctrl_mode == CTRL_NONE) {
      input_current = 0;
    } else if (motor->ctrl_mode == CTRL_CURRENT) {
      input_current = motor->ref_current;
    } else if (motor->ctrl_mode == CTRL_VELOCITY) {
      input_current =
          motor->kp_spd * (motor->ref_speed_rpm - motor->cur_speed_rpm);
    } else if (motor->ctrl_mode == CTRL_POSITION) {
      int kp_current_input =
          motor->kp_ang * (motor->ref_total_angle - motor->cur_total_angle);
      // clip kp_current_input to -kp_ang_max_current ~ kp_ang_max_current
      kp_current_input =
          clip_value(kp_current_input, -motor->kp_ang_max_current,
                     motor->kp_ang_max_current);
      int kd_current_input = motor->kd_ang * (0 - motor->cur_speed_rpm);
      // clip kd_current_input to -kd_ang_max_current ~ kd_ang_max_current
      kd_current_input =
          clip_value(kd_current_input, -motor->kd_ang_max_current,
                     motor->kd_ang_max_current);
      input_current = kp_current_input + kd_current_input;
    } else if (motor->ctrl_mode == CTRL_STATE) {
      int kp_current_input =
          motor->kp_ang * (motor->ref_total_angle - motor->cur_total_angle);
      // clip kp_current_input to -kp_ang_max_current ~ kp_ang_max_current
      kp_current_input =
          clip_value(kp_current_input, -motor->kp_ang_max_current,
                     motor->kp_ang_max_current);
      int kd_current_input =
          motor->kd_ang * (motor->ref_speed_rpm - motor->cur_speed_rpm);
      // clip kd_current_input to -kd_ang_max_current ~ kd_ang_max_current
      kd_current_input =
          clip_value(kd_current_input, -motor->kd_ang_max_current,
                     motor->kd_ang_max_current);
      input_current = kp_current_input + kd_current_input;
    }
    message[2 * id - 2] = (input_current >> 8);
    message[2 * id - 1] = input_current;
  }
}

void get_motor_command_feedback(char *msg) {
  // 配列の開始ブラケット
  *msg++ = '[';

  // motors_can1配列をイテレート
  for (int i = 0; i < 4; ++i) {
    // 各モーターの情報をJSON形式でsprintf
    msg += sprintf(msg, "{\"cur\":%d,\"spd\":%d,\"ang\":%ld}",
                   motors_can1[i].cur_current, motors_can1[i].cur_speed_rpm,
                   motors_can1[i].cur_total_angle);

    // 最後の要素以外にはカンマを追加
    if (i < 3) {
      *msg++ = ',';
    }
  }

  // 配列の終了ブラケット
  *msg++ = ']';
  *msg = '\0';  // NULL終端
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  int level = HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0);

  // if (level == 0) {
  //   HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
  //   HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
  // } else if (level == 1) {
  //   HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
  //   HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
  // } else if (level > 1) {
  //   HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
  //   HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
  // }

  // read from CAN FIFO
  for (int i = 0; i < level; i++) {
    CAN_RxHeaderTypeDef header;
    uint8_t message[8];
    HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &header, message);
    parse_motor_rx_message(&header, message);
  }
}