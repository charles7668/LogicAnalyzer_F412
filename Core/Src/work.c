//#include <usbd_cdc_if.h>
#include <cmsis_os2.h>
#include <stdio.h>
#include <string.h>
#include "work.h"
#include "main.h"
#include "FreeRTOS.h"

#define MAX_SAMPLE_DATA_COUNT 200000

extern TIM_HandleTypeDef htim2;
extern uint32_t counter;
extern UART_HandleTypeDef huart3;
extern osThreadId_t read_signal_id;
extern osMessageQueueId_t messageQueueId;
uint8_t test[MAX_SAMPLE_DATA_COUNT] = {0};
typedef struct {
    uint32_t data;
} Message_Obj_t;

/**
 * @brief delay with timer counter
 * @param us
 */
void delay_us(uint32_t us)
{
  uint32_t t1 = __HAL_TIM_GET_COUNTER(&htim2);
  while (__HAL_TIM_GET_COUNTER(&htim2) - t1 < us);
}

/**
 * @brief read signal thread
 * @param param void
 */
void ReadSignalThread(void *param)
{
  for (;;) {
    osThreadFlagsWait(1U, osFlagsWaitAny, osWaitForever);
    Message_Obj_t msg;
    osMessageQueueGet(messageQueueId, &msg, 0, 10);
    uint32_t interval = msg.data;
    osMessageQueueGet(messageQueueId, &msg, 0, 10);
    uint32_t sample_data_count = msg.data;
    if (sample_data_count > MAX_SAMPLE_DATA_COUNT) sample_data_count = MAX_SAMPLE_DATA_COUNT;
    counter = 0;
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    HAL_TIM_Base_Start(&htim2);
    uint32_t ms_t1 = osKernelGetTickCount();
    while (counter < sample_data_count) {
      delay_us(interval);
      test[counter] = *(__IO uint8_t *) (0x60000000);
      counter++;
    }
    uint32_t ms_t2 = osKernelGetTickCount();
    HAL_TIM_Base_Stop(&htim2);
    uint8_t temp_str[20];
    memset(temp_str, 0, 20);
    sprintf(temp_str, "%d %d\n", interval, ms_t2 - ms_t1);
    HAL_UART_Transmit(&huart3, temp_str, strlen(temp_str), 1000);
    HAL_UART_Transmit(&huart3, test, sample_data_count, sample_data_count * 10);
    osThreadFlagsClear(1U);
  }
}

/**
 * @brief wait command
 * @param param void
 */
void WaitCommandThread(void *param)
{
  HAL_GPIO_WritePin(TEST_SIGNAL_GPIO_Port, TEST_SIGNAL_Pin, GPIO_PIN_SET);
  for (;;) {
    uint8_t receive[13] = {0};
    memset(receive, 0, 13);
    uint8_t state = HAL_UART_Receive(&huart3, receive, 13, 1000);
    if (state == HAL_OK) {
      osDelay(1);
      uint8_t token[6] = {0};
      memcpy(token, receive, 5);
      if (strcmp(token, "start") == 0) {
        uint8_t counter = 0;
        uint8_t start_index = 5;
        while (counter < 2) {
          Message_Obj_t msg;
          memcpy(&msg.data, receive + start_index, 4);
          osMessageQueuePut(messageQueueId, &msg, 0, 10);
          start_index += 4;
          counter++;
        }
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
