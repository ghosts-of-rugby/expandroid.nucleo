/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"

#include "cmsis_os.h"
#include "main.h"
#include "task.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can.h"
#include "expandroid_can.h"
#include "expandroid_config.h"
#include "expandroid_led.h"
#include "expandroid_robomaster.h"
#include "expandroid_udp.h"
#include "lwip/sockets.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for ledTask */
osThreadId_t ledTaskHandle;
uint32_t ledTaskBuffer[1024];
osStaticThreadDef_t ledTaskControlBlock;
const osThreadAttr_t ledTask_attributes = {
    .name = "ledTask",
    .cb_mem = &ledTaskControlBlock,
    .cb_size = sizeof(ledTaskControlBlock),
    .stack_mem = &ledTaskBuffer[0],
    .stack_size = sizeof(ledTaskBuffer),
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for userBtnIrqClien */
osThreadId_t userBtnIrqClienHandle;
uint32_t userBtnIrqClienBuffer[128];
osStaticThreadDef_t userBtnIrqClienControlBlock;
const osThreadAttr_t userBtnIrqClien_attributes = {
    .name = "userBtnIrqClien",
    .cb_mem = &userBtnIrqClienControlBlock,
    .cb_size = sizeof(userBtnIrqClienControlBlock),
    .stack_mem = &userBtnIrqClienBuffer[0],
    .stack_size = sizeof(userBtnIrqClienBuffer),
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for MotorControlTas */
osThreadId_t MotorControlTasHandle;
uint32_t MotorControlTasBuffer[1024];
osStaticThreadDef_t MotorControlTasControlBlock;
const osThreadAttr_t MotorControlTas_attributes = {
    .name = "MotorControlTas",
    .cb_mem = &MotorControlTasControlBlock,
    .cb_size = sizeof(MotorControlTasControlBlock),
    .stack_mem = &MotorControlTasBuffer[0],
    .stack_size = sizeof(MotorControlTasBuffer),
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for MotorCmdTask */
osThreadId_t MotorCmdTaskHandle;
uint32_t MotorCmdTaskBuffer[1024];
osStaticThreadDef_t MotorCmdTaskControlBlock;
const osThreadAttr_t MotorCmdTask_attributes = {
    .name = "MotorCmdTask",
    .cb_mem = &MotorCmdTaskControlBlock,
    .cb_size = sizeof(MotorCmdTaskControlBlock),
    .stack_mem = &MotorCmdTaskBuffer[0],
    .stack_size = sizeof(MotorCmdTaskBuffer),
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for udpEchoBackTask */
osThreadId_t udpEchoBackTaskHandle;
uint32_t udpEchoBackTaskBuffer[128];
osStaticThreadDef_t udpEchoBackTaskControlBlock;
const osThreadAttr_t udpEchoBackTask_attributes = {
    .name = "udpEchoBackTask",
    .cb_mem = &udpEchoBackTaskControlBlock,
    .cb_size = sizeof(udpEchoBackTaskControlBlock),
    .stack_mem = &udpEchoBackTaskBuffer[0],
    .stack_size = sizeof(udpEchoBackTaskBuffer),
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for statusMessageQueue */
osMessageQueueId_t statusMessageQueueHandle;
uint8_t statusMessageQueueBuffer[16 * sizeof(uint8_t)];
osStaticMessageQDef_t statusMessageQueueControlBlock;
const osMessageQueueAttr_t statusMessageQueue_attributes = {
    .name = "statusMessageQueue",
    .cb_mem = &statusMessageQueueControlBlock,
    .cb_size = sizeof(statusMessageQueueControlBlock),
    .mq_mem = &statusMessageQueueBuffer,
    .mq_size = sizeof(statusMessageQueueBuffer)};
/* Definitions for buttonPressedEvent */
osEventFlagsId_t buttonPressedEventHandle;
const osEventFlagsAttr_t buttonPressedEvent_attributes = {
    .name = "buttonPressedEvent"};
/* Definitions for motorInitEvent */
osEventFlagsId_t motorInitEventHandle;
const osEventFlagsAttr_t motorInitEvent_attributes = {.name = "motorInitEvent"};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartLedTask(void *argument);
void StartUserBtnIrqClientInit(void *argument);
void MotorControlTask(void *argument);
void MotorCommandTask(void *argument);
void udpEchoBack(void *argument);

extern void MX_LWIP_Init(void);
extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of statusMessageQueue */
  statusMessageQueueHandle =
      osMessageQueueNew(16, sizeof(uint8_t), &statusMessageQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle =
      osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of ledTask */
  ledTaskHandle = osThreadNew(StartLedTask, NULL, &ledTask_attributes);

  /* creation of userBtnIrqClien */
  userBtnIrqClienHandle =
      osThreadNew(StartUserBtnIrqClientInit, NULL, &userBtnIrqClien_attributes);

  /* creation of MotorControlTas */
  MotorControlTasHandle =
      osThreadNew(MotorControlTask, NULL, &MotorControlTas_attributes);

  /* creation of MotorCmdTask */
  MotorCmdTaskHandle =
      osThreadNew(MotorCommandTask, NULL, &MotorCmdTask_attributes);

  /* creation of udpEchoBackTask */
  udpEchoBackTaskHandle =
      osThreadNew(udpEchoBack, NULL, &udpEchoBackTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of buttonPressedEvent */
  buttonPressedEventHandle = osEventFlagsNew(&buttonPressedEvent_attributes);

  /* creation of motorInitEvent */
  motorInitEventHandle = osEventFlagsNew(&motorInitEvent_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
  /* init code for LWIP */
  MX_LWIP_Init();

  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  osThreadExit();
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartLedTask */
/**
 * @brief Function implementing the ledTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLedTask */
void StartLedTask(void *argument) {
  /* USER CODE BEGIN StartLedTask */
  int sock;

  struct sockaddr_in client_address;
  socklen_t client_address_len;

  create_udp_server(PORT_LED, &sock, &client_address, &client_address_len,
                    SOCKET_BLOCKING);

  client_address_len = sizeof(client_address);

  for (;;) {
    char recv_buffer[128];
    int bytes_read =
        lwip_recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0,
                      (struct sockaddr *)&client_address, &client_address_len);
    if (bytes_read > 0) {
      // Null-terminate the received data
      recv_buffer[bytes_read] = '\0';

      // Parse the JSON data
      cJSON *json = cJSON_Parse(recv_buffer);
      if (json != NULL) {
        cJSON *led2 = cJSON_GetObjectItem(json, "led2");
        control_led_from_json(led2, LD2_GPIO_Port, LD2_Pin);
        cJSON *led3 = cJSON_GetObjectItem(json, "led3");
        control_led_from_json(led3, LD3_GPIO_Port, LD3_Pin);
        cJSON_Delete(json);
      }
    }
    // sleep
    osDelay(10);
  }
  /* USER CODE END StartLedTask */
}

/* USER CODE BEGIN Header_StartUserBtnIrqClientInit */
/**
 * @brief Function implementing the userBtnIrqClien thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartUserBtnIrqClientInit */
void StartUserBtnIrqClientInit(void *argument) {
  /* USER CODE BEGIN StartUserBtnIrqClientInit */
  server_info server;
  char msg_buffer[128];
  create_udp(&server, PORT_BTN, msg_buffer, sizeof(msg_buffer));

  for (;;) {
    osEventFlagsWait(buttonPressedEventHandle, 0x00000001U, osFlagsWaitAll,
                     osWaitForever);
    uint32_t time = osKernelGetTickCount();
    char message[50];
    sprintf(message, "{\"time\":%lu}", time);
    send_udp(&server, message, strlen(message));
  }
  /* USER CODE END StartUserBtnIrqClientInit */
}

/* USER CODE BEGIN Header_MotorControlTask */
/**
 * @brief Function implementing the MotorControlTas thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_MotorControlTask */
void MotorControlTask(void *argument) {
  /* USER CODE BEGIN MotorControlTask */
  start_can(&hcan1);
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) !=
      HAL_OK) {
    Error_Handler();
  }
  /* Infinite loop */
  for (;;) {
    CAN_TxHeaderTypeDef header;
    uint8_t message[8] = {0};
    get_motor_tx_message(&header, message);
    HAL_CAN_AddTxMessage(&hcan1, &header, message, NULL);
    osDelay(1);
  }
  /* USER CODE END MotorControlTask */
}

/* USER CODE BEGIN Header_MotorCommandTask */
/**
 * @brief Function implementing the MotorCmdTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_MotorCommandTask */
void MotorCommandTask(void *argument) {
  /* USER CODE BEGIN MotorCommandTask */
  server_info server;
  char msg_buffer[512];

  create_udp(&server, PORT_MOTOR, msg_buffer, sizeof(msg_buffer));

  nonblocking_udp(&server);

  init_motors();

  while (1) {
    /* Infinite loop */
    recv_dtype type = recv_udp(&server);
    if (type == RECV_DATA) {
      cJSON *json = cJSON_Parse(server.msg_buffer);
      if (json != NULL) {
        process_motor_command(json);
        cJSON_Delete(json);
      }
    }
    get_motor_command_feedback(server.msg_buffer);
    send_udp(&server, server.msg_buffer, strlen(server.msg_buffer));
    osDelay(10);
  }
  /* USER CODE END MotorCommandTask */
}

/* USER CODE BEGIN Header_udpEchoBack */
/**
 * @brief Function implementing the udpEchoBackTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_udpEchoBack */
void udpEchoBack(void *argument) {
  /* USER CODE BEGIN udpEchoBack */
  // server_info server;
  // char recv_buffer[128];

  // create_udp(&server, PORT_MOTOR, recv_buffer, sizeof(recv_buffer));
  for (;;) {
    osDelay(1);
  }
  /* USER CODE END udpEchoBack */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
