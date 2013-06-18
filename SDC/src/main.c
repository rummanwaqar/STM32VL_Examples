#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "spi.h"
#include "mmc.h"
#include "uart.h"

void Delay(uint32_t nTimes);

int main(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  if (SysTick_Config(SystemCoreClock / 1000))
    while (1);

  uart_open(USART1, 38400);

  while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5));
  Delay(500);
  
  uart_puts("\r\n", USART1);

  uart_puts("CONNECTED\n\r", USART1);
  Delay(500);
  
  if(disk_initialize() != 0) {
    uart_puts("DISK INITIALIZED\n\r", USART1);
  } else {
    uart_puts("INITIALIZATION ERROR\n\r", USART1);
  }

  uint8_t out[512], in[512];
  int i;

  // output Hello ++++...
  for (i=0; i<512; i++)
    out[i] = 0x2A;

  out[0] = 0x48;
  out[1] = 0x45;
  out[2] = 0x4C;
  out[3] = 0x4C;
  out[4] = 0x4F;

  //for(i=0; i<512;i++)
  if(disk_write(out, 0) == 0) {
    uart_puts("ERROR WRITING BLOCK\n\r", USART1);
  }

  // read the card
  if(!disk_read(in, 0)) {
    uart_puts("ERROR READING BLOCK\n\r", USART1);
  } else {
    uart_puts("READING ... \n\r", USART1);
  }
  
  for(i=0; i<512; i++)
    uart_putc(in[i], USART1);

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
