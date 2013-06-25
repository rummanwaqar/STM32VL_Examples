#ifndef __LCD_H
#define __LCD_H

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "delay.h"

// Pin Defines
#define LCD_EN GPIO_Pin_3
#define LCD_RW GPIO_Pin_2
#define LCD_RS GPIO_Pin_1

#define LCD_CMD GPIOA
#define LCD_DATA GPIOC
#define LCD_CMDCLOCK RCC_APB2Periph_GPIOA
#define LCD_DATACLOCK RCC_APB2Periph_GPIOC

// Public Functions
void lcd_init(void);
void lcd_string( uint8_t * data );
void lcd_goto(uint8_t x, uint8_t y);
void lcd_command( uint8_t cmd );
void lcd_char( uint8_t data );

// Private Functions
static void lcd_lowLevelInit(void);
static void lcd_nybble(void);
#endif
