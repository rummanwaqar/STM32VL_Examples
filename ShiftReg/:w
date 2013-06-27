#ifndef __SHIFT_REG_H
#define __SHIFT_REG_H

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#define SHIFT_SH_CP GPIO_Pin_10 
#define SHIFT_ST_CP GPIO_Pin_11
#define SHIFT_DS GPIO_Pin_12

#define SHIFT_PORT GPIOC
#define SHIFT_RCC RCC_APB2Periph_GPIOC

void shiftReg_init( void );
void shiftReg_send( uint8_t data );

#endif
