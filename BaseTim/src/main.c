#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>

void tim_init(void);

int main(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  tim_init();

  int ledVal = 0;  
  while(1) {
    if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) != RESET) {
      TIM_ClearFlag(TIM2, TIM_IT_Update);
      ledVal = 1 - ledVal;
      GPIO_WriteBit(GPIOC, GPIO_Pin_6, (ledVal) ? Bit_SET : Bit_RESET);
    }
  }
}

void tim_init (void) {
  TIM_TimeBaseInitTypeDef TIM_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructInit(&TIM_InitStructure);
  TIM_InitStructure.TIM_Prescaler = 1;
  TIM_InitStructure.TIM_Period = 200 - 1;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_InitStructure);

  TIM_Cmd(TIM2, ENABLE);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
  /* Infinite loop */
  while(1);
}
#endif
