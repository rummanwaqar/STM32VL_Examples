#include "shiftReg.h"

void shiftReg_init() {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd( SHIFT_RCC, ENABLE );

  GPIO_StructInit( &GPIO_InitStructure );
  GPIO_InitStructure.GPIO_Pin = SHIFT_SH_CP | SHIFT_ST_CP | SHIFT_DS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init( GPIOC, &GPIO_InitStructure );
}

void shiftReg_send( uint8_t data  ) {
  uint8_t i, j;

  // set SH_CP low
  SHIFT_PORT->BRR = SHIFT_SH_CP;
  // set ST_CP low
  SHIFT_PORT->BRR = SHIFT_ST_CP;
  
  for( i=0; i<8; i++ ) {
    // set DS bit
    if( data & 0x1 )
      SHIFT_PORT->BSRR = SHIFT_DS;
    else
      SHIFT_PORT->BRR = SHIFT_DS;
    data = data >> 1;
    // pulse SH_CP
    SHIFT_PORT->BSRR = SHIFT_SH_CP;
    for( j=0; j<5; j++ );
    SHIFT_PORT->BRR = SHIFT_SH_CP;
  }
  // set ST_CP high
  SHIFT_PORT->BSRR = SHIFT_ST_CP;
}

