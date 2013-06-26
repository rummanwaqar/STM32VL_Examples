#include "delay.h"

// Timer delays
void delay_us( uint16_t delay ) {
  TIM6->PSC = 24 - 1;
  TIM6->ARR = delay;
  TIM6->DIER = TIM_DIER_UIE;
  TIM6->SR &= ~TIM_SR_UIF;
  TIM6->CR1 &= ~TIM_CR1_ARPE;
  TIM6->SR = 0x00;
  TIM6->CR1 |= TIM_CR1_CEN;

  while( TIM6->SR == 0 );
}

void delay_ms( uint16_t delay ) {
  uint16_t i;
  for( i=0; i<delay; i++ ) delay_us(1000);
}

