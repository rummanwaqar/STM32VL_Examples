#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "spi.h"

uint8_t txbuf[4], rxbuf[4];
void Delay(uint32_t nTimes);

int main(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  spi_init();

  if (SysTick_Config(SystemCoreClock / 1000))
    while (1);

  Delay(2000);

  int i, j;

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 4; j++) {
      txbuf[j] = i * 4 + j;
    }
    GPIO_WriteBit(GPIOC, GPIO_Pin_3, 0);
    spi_readWrite(rxbuf, txbuf, 4 );
    GPIO_WriteBit(GPIOC, GPIO_Pin_3, 1);
    for (j = 0; j < 4; j++)
      if(rxbuf[j] != txbuf[j])
        assert_failed(__FILE__, __LINE__);
  }

  while(1) {
    static int ledVal = 0;
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, (ledVal) ? Bit_SET : Bit_RESET);
    ledVal = 1 - ledVal;
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
