#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "lcd.h"
#include "delay.h"

/* MAIN -----------------------------*/
int main(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE );

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  lcd_init();
  lcd_goto( 8, 2 );
  lcd_string( "ARVP" );
  lcd_goto( 1, 3 );
  lcd_string( "Autonomous Robotics" );
  lcd_goto( 3, 4 );
  lcd_string( "Vehicle Project");

  while(1) {
    static int ledVal = 0;
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, (ledVal) ? Bit_SET : Bit_RESET);
    ledVal = 1 - ledVal;
    delay_ms(200);
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
  /* Infinite loop */
  while(1);
}
#endif
