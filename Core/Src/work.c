//#include <usbd_cdc_if.h>
#include <cmsis_os2.h>
#include <stdio.h>
#include <string.h>
#include "work.h"
#include "main.h"
#include "FreeRTOS.h"

extern TIM_HandleTypeDef htim2;
extern uint32_t counter;
extern UART_HandleTypeDef huart3;
extern osThreadId_t read_signal_id;
uint8_t test[200000] = {0};

/**
 * @brief delay with timer counter
 * @param us
 */
void delay_us(uint32_t us)
{
  uint32_t t1 = __HAL_TIM_GET_COUNTER(&htim2);
  while (__HAL_TIM_GET_COUNTER(&htim2) - t1 < 1);
}

/**
 * @brief read signal thread
 * @param param void
 */
void ReadSignalThread(void *param)
{
  for (;;) {
    osThreadFlagsWait(1U, osFlagsWaitAny, osWaitForever);
    counter = 0;
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    HAL_TIM_Base_Start(&htim2);
    uint32_t ms_t1 = osKernelGetTickCount();
    while (counter < 200000) {
      delay_us(1);
      test[counter] = *(__IO uint8_t *) (0x60000000);
      counter++;
    }
    uint32_t ms_t2 = osKernelGetTickCount();
    HAL_TIM_Base_Stop(&htim2);
    uint8_t temp[20];
    memset(temp, 0, 20);
    sprintf(temp, "%d %d\n", ms_t2 - ms_t1, test[100]);
    HAL_UART_Transmit(&huart3, temp, 20, 1000);
    osThreadFlagsClear(1U);
  }
}

/**
 * @brief wait command
 * @param param void
 */
void WaitCommandThread(void *param)
{
  for (;;) {
    uint8_t receive[12] = {0};
    memset(receive, 0, 12);
    uint8_t state = HAL_UART_Receive(&huart3, receive, 12, 1000);
    if (state == HAL_OK) {
      osDelay(1);
      char *token = strtok(receive, (const char *) " ");
      if (strcmp(token, "start") == 0) {
        HAL_UART_Transmit(&huart3, token, 6, 1000);
        token = strtok(NULL, (const char *) " ");
        if (token != NULL)
          HAL_UART_Transmit(&huart3, token, strlen(token) + 1, 1000);
        osThreadFlagsSet(read_signal_id, 1U);
      }
    }
    osDelay(1);
  }
}

/**
 * @brief output test signal (just for test)
 * @param param void
 */
void TestSignalToggleThread(void *param)
{
  uint32_t tick2s = 1000 * portTICK_PERIOD_MS;
  for (;;) {
    HAL_GPIO_TogglePin(TEST_SIGNAL_GPIO_Port, TEST_SIGNAL_Pin);
    osDelay(tick2s);
  }
}
