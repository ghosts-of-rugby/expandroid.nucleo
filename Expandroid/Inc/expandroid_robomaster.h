#ifndef __EXPANDROID_ROBOMASTER_H__
#define __EXPANDROID_ROBOMASTER_H__
#include "cJSON/cJSON.h"
#include "can.h"

#define ROBOMASTER_TX_ID 0x200
#define MAX_MOTOR_NUM_PER_CANBUS 4

typedef enum __control_mode {
  CTRL_NONE,
  CTRL_CURRENT,
  CTRL_VELOCITY,
  CTRL_POSITION,
  CTRL_STATE,
} control_mode;

typedef struct __robomaster_motor {
  uint8_t initialized;

  control_mode ctrl_mode;

  int16_t ref_current;
  int16_t cur_current;

  int16_t ref_speed_rpm;
  int16_t cur_speed_rpm;

  int32_t ref_total_angle;
  int32_t cur_total_angle;
  uint16_t cur_angle;
  uint16_t prev_angle;
  uint16_t offset_angle;
  int32_t round_cnt;

  // control parameters
  float kp_spd;
  float kp_ang;
  float kd_ang;
  float kp_ang_max_current;
  int32_t kd_ang_max_current;

  // // observer
  // float obs_speed_rpm;
  // float obs_dist;
  // float acc_per_current;
  // float gain_speed;
  // float gain_dist;
} robomaster_motor;

extern robomaster_motor motors_can1[MAX_MOTOR_NUM_PER_CANBUS];

void init_motors();

void process_motor_command(cJSON *json);

void parse_motor_rx_message(CAN_RxHeaderTypeDef *header, uint8_t *message);

void get_motor_command_feedback(char *msg);

void get_motor_tx_message(CAN_TxHeaderTypeDef *header, uint8_t *message);

#endif /* __EXPANDROID_ROBOMASTER_H__ */