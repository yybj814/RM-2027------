/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "oled.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "stm32f1xx_hal_uart.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
static void UIntToStr(uint16_t v, char *buf)
{
  char tmp[8];
  int i = 0, j = 0;
  if (v == 0) { buf[0] = '0'; buf[1] = 0; return; }
  while (v > 0) { tmp[i++] = (char)('0' + v % 10); v /= 10; }
  while (i > 0) buf[j++] = tmp[--i];
  buf[j] = 0;
}

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
volatile float gAngle = 0.0f;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* 角度 → CCR：0°=500us，180°=2500us，1计数=1us */


static uint16_t AngleToCCR(float angle)
{
  if (angle < 2.0f)   angle = 2.0f;    //两端留余量
  if (angle > 178.0f) angle = 178.0f;
  return (uint16_t)(500.0f + angle / 180.0f * 2000.0f);
}

static void Servo_SetAngle(float angle)
{
  uint16_t ccr = AngleToCCR(angle);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ccr);
}

/* OLED 三行：Target / CCR / Pulse */
static void OLED_ShowServo(float angle)//屏幕
{
  uint16_t ccr = AngleToCCR(angle);
  char line[20];

  OLED_NewFrame();
  strcpy(line, "Target:");
  UIntToStr((uint16_t)angle, line + 7);
  strcat(line, "deg");
  OLED_PrintASCIIString(0, 0, line, &afont16x8, OLED_COLOR_NORMAL);

  strcpy(line, "CCR:");
  UIntToStr(ccr, line + 4);
  OLED_PrintASCIIString(0, 16, line, &afont16x8, OLED_COLOR_NORMAL);

  strcpy(line, "Pulse:");
  UIntToStr(ccr / 1000, line + 6);
  strcat(line, ".");
  UIntToStr(ccr % 1000, line + 8);
  strcat(line, "ms");
  OLED_PrintASCIIString(0, 32, line, &afont16x8, OLED_COLOR_NORMAL);

  OLED_ShowFrame();

  printf("Target=%3d  CCR=%4d  Pulse=%d.%03dms\r\n",//串口发送
         (int)angle, ccr, ccr / 1000, ccr % 1000);
}

/* USER CODE END Variables */
/* Definitions for deSG90 */
osThreadId_t deSG90Handle;
const osThreadAttr_t deSG90_attributes = {
  .name = "deSG90",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for KEY1 */
osThreadId_t KEY1Handle;
const osThreadAttr_t KEY1_attributes = {
  .name = "KEY1",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for KEY2 */
osThreadId_t KEY2Handle;
const osThreadAttr_t KEY2_attributes = {
  .name = "KEY2",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartSG90(void *argument);
void StartKEY1(void *argument);
void StartKEY2(void *argument);

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
  /* creation of deSG90 */
  deSG90Handle = osThreadNew(StartSG90, NULL, &deSG90_attributes);

  /* creation of KEY1 */
  KEY1Handle = osThreadNew(StartKEY1, NULL, &KEY1_attributes);

  /* creation of KEY2 */
  KEY2Handle = osThreadNew(StartKEY2, NULL, &KEY2_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartSG90 */
/**
  * @brief  Function implementing the deSG90 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartSG90 */


void StartSG90(void *argument)
{
  /* USER CODE BEGIN StartSG90 */
  const float seq[5] = {0.0f, 45.0f, 90.0f, 135.0f, 180.0f};
  for(;;)
  {
    for (int i = 0; i < 5; i++)
    {
      gAngle = seq[i];
      Servo_SetAngle(gAngle);
      OLED_ShowServo(gAngle);
      osDelay(1000);
    }
  }
  /* USER CODE END StartSG90 */
}





/* USER CODE BEGIN Header_StartKEY1 */
/**
* @brief Function implementing the KEY1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKEY1 */
void StartKEY1(void *argument) {
  /* USER CODE BEGIN StartKEY1 */
  uint8_t last = 1;
  for(;;)
  {
    uint8_t cur = HAL_GPIO_ReadPin(GPIOB, KEY__Pin);
    if (cur == GPIO_PIN_RESET && last == GPIO_PIN_SET)
    {
      osDelay(20); // 消抖
      gAngle += 15.0f;
      if (gAngle > 180.0f) gAngle = 180.0f;   // 限幅
      Servo_SetAngle(gAngle);
      OLED_ShowServo(gAngle);
    }
    last = cur;
    osDelay(10);
  }
  /* USER CODE END StartKEY1 */
}
/**
* @brief Function implementing the KEY2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKEY2 */
void StartKEY2(void *argument)
{
  /* USER CODE BEGIN StartKEY2 */
  uint8_t last = 1;
  for(;;)
  {
    uint8_t cur = HAL_GPIO_ReadPin(GPIOB, KEY_B15_Pin);
    if (cur == GPIO_PIN_RESET && last == GPIO_PIN_SET)
    {
      osDelay(20);
      gAngle -= 15.0f;
      if (gAngle < 0.0f) gAngle = 0.0f;
      Servo_SetAngle(gAngle);
      OLED_ShowServo(gAngle);
    }
    last = cur;
    osDelay(10);
  }
  /* USER CODE END StartKEY2 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

