#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "uart.h"

void Delay(uint32_t nTimes);

int main(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // open a uart connection
  uart_open(USART1, 38400);

  // set up sys_tick interrupt for Delay()
  if (SysTick_Config(SystemCoreClock / 1000))
    while (1);

  
  int ledVal = 0;
  char inp[28];
  while(1) {
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, (ledVal) ? Bit_SET : Bit_RESET);
    ledVal = 1 - ledVal;
    uart_puts("HELLO WORLD\r\n", USART1);
    Delay(250);
  }
}

static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime) {
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

void SysTick_Handler(void) {
  if (TimingDelay != 0x00) {
    TimingDelay--;
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
  /* Infinite loop */
  while(1);
}
#endif
