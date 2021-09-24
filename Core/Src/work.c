//
// Created by charles on 2021/9/24.
//

#include <usbd_cdc_if.h>
#include <cmsis_os2.h>
#include "work.h"
#include "main.h"
#include "FreeRTOS.h"

extern TIM_HandleTypeDef htim2;
void GPIO_ReadSignal(void *param)
{
  uint32_t tick1s = 1000 * portTICK_PERIOD_MS;
  GPIO_PinState pre_state = GPIO_PIN_RESET;
  uint32_t counter = 0;
  osThreadId_t * id = (osThreadId_t *)(param);
  HAL_TIM_Base_Start_IT(&htim2);
  for (;;) {
    osThreadFlagsWait(1U,osFlagsWaitAny,osWaitForever);
    GPIO_PinState state = HAL_GPIO_ReadPin(READ_PIN_GPIO_Port, READ_PIN_Pin);
    uint8_t send_message[3] = "0\n";
    send_message[0] = '0' + state;
    if (pre_state != state) {
      char temp[100];
      sprintf(temp, "%d", counter);
      CDC_Transmit_FS(temp, strlen(temp));
      counter = 0;
    }
    counter++;
    pre_state = state;
    osThreadFlagsClear(0U);
//    CDC_Transmit_FS(send_message, 2);
//    osDelay(1);
  }
}

void TEST_SIGNAL_Toggle(void *param)
{
  uint32_t tick2s = 1000 * portTICK_PERIOD_MS;
  for (;;) {
    HAL_GPIO_TogglePin(TEST_SIGNAL_GPIO_Port, TEST_SIGNAL_Pin);
    osDelay(tick2s);
  }
}
