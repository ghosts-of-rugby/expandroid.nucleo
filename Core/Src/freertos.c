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
#include "connections.h"
#include "lwip/sockets.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
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
/* Definitions for Can01Task */
osThreadId_t Can01TaskHandle;
uint32_t Can01TaskBuffer[1024];
osStaticThreadDef_t Can01TaskControlBlock;
const osThreadAttr_t Can01Task_attributes = {
    .name = "Can01Task",
    .cb_mem = &Can01TaskControlBlock,
    .cb_size = sizeof(Can01TaskControlBlock),
    .stack_mem = &Can01TaskBuffer[0],
    .stack_size = sizeof(Can01TaskBuffer),
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for buttonPressedEvent */
osEventFlagsId_t buttonPressedEventHandle;
const osEventFlagsAttr_t buttonPressedEvent_attributes = {
    .name = "buttonPressedEvent"};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartLedTask(void *argument);
void StartUserBtnIrqClientInit(void *argument);
void StartCan01Task(void *argument);

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

  /* creation of Can01Task */
  Can01TaskHandle = osThreadNew(StartCan01Task, NULL, &Can01Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of buttonPressedEvent */
  buttonPressedEventHandle = osEventFlagsNew(&buttonPressedEvent_attributes);

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

  create_udp_server(PORT_LED, &sock, &client_address, &client_address_len);

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
        control_led_from_json(json);
        cJSON_Delete(json);
      }
    }
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
  int sock;

  struct sockaddr_in client_address;
  socklen_t client_address_len;

  create_udp_server(PORT_USER_Btn_IRQ, &sock, &client_address,
                    &client_address_len);

  for (;;) {
    osEventFlagsWait(buttonPressedEventHandle, 0x00000001U, osFlagsWaitAll,
                     osWaitForever);
    uint32_t time = osKernelGetTickCount();
    char message[50];
    sprintf(message, "{\"time\":%lu}", time);
    lwip_sendto(sock, message, strlen(message), 0,
                (struct sockaddr *)&client_address, client_address_len);
    // HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  }
  /* USER CODE END StartUserBtnIrqClientInit */
}

/* USER CODE BEGIN Header_StartCan01Task */
/**
 * @brief Function implementing the Can01Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartCan01Task */
void StartCan01Task(void *argument) {
  /* USER CODE BEGIN StartCan01Task */
  int sock;

  struct sockaddr_in client_address;
  socklen_t client_address_len;

  create_udp_server(PORT_CAN01, &sock, &client_address, &client_address_len);

  lwip_ioctl(sock, FIONBIO, 1);  // set non-blocking

  /* Infinite loop */
  for (;;) {
    char recv_buffer[128];
    int bytes_read =
        lwip_recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0,
                      (struct sockaddr *)&client_address, &client_address_len);
    // HAL_CAN_Start(&hcan1);
    // uint8_t message[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    // CAN_TxHeaderTypeDef TxHeader;

    // TxHeader.DLC = 8;             // Data length of 8
    // TxHeader.StdId = 0x123;       // Standard ID
    // TxHeader.IDE = CAN_ID_STD;    // Standard ID
    // TxHeader.RTR = CAN_RTR_DATA;  // Data frame

    // uint32_t mailbox;
    // if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, message, &mailbox) != HAL_OK)
    // {
    //   // Transmission request failed
    //   // Error_Handler();
    //   HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    // } else {
    //   HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    // }
    // HAL_CAN_AddTxMessage(&hcan1, &TxHeader, message, &mailbox);
    // HAL_CAN_ConfigFilter(&hcan1);
    osDelay(1);
  }
  /* USER CODE END StartCan01Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
