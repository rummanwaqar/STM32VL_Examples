#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "shiftReg.h"
#include "delay.h"

int main(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  shiftReg_init();
  uint8_t data[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  shiftReg_send( data );
  
  uint8_t ledVal = 0;
  
  while(1) {
    GPIO_WriteBit( GPIOC, GPIO_Pin_9, (ledVal) ? Bit_SET : Bit_RESET );
    ledVal = 1 - ledVal;
    delay_ms( 100 );
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
  /* Infinite loop */
  while(1);
}
#endif
