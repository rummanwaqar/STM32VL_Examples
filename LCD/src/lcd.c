#include "lcd.h"

uint8_t lcd_ColPos[4] = {0, 64, 20, 84};

// Public Functions
void lcd_init() {
  lcd_lowLevelInit();
  GPIO_ResetBits( LCD_CMD, LCD_RS | LCD_RW | LCD_EN );
  GPIO_ResetBits( LCD_DATA, 0xFF );
  delay_ms(40);
  lcd_command( 0x38 ); // function set
  delay_ms( 5 );
  lcd_command( 0x38 ); // function set
  delay_ms( 1 );
  lcd_command( 0x08 ); // display off
  lcd_command( 0x01 ); // clear display
  lcd_command( 0x06 ); // entry mode 
  lcd_command( 0x02 ); // home command
  lcd_command( 0x0C ); // display on 
}

void lcd_goto( uint8_t x, uint8_t y ) {
  lcd_command( 0x80 + lcd_ColPos[y-1] + x-1 );;
}

void lcd_string( uint8_t * data ) {
  while( *data > 0 ) lcd_char( *data++ );
}

void lcd_char( uint8_t data ) {
  delay_us(40);
  LCD_CMD->BSRR = LCD_RS;       // RS=1 : Data
  LCD_CMD->BRR = LCD_RW;        // RS=0 : Write
  LCD_DATA->ODR = ( LCD_DATA->ODR & 0xFF00 ) | ( data & 0x00FF ); // put data on data port
  lcd_nybble();
}

void lcd_command( uint8_t cmd ) {
  delay_us( 40 );
  LCD_DATA->ODR =( LCD_DATA->ODR & 0xFF00 ) | ( cmd & 0x00FF ); // put data on data port
  LCD_CMD->BRR = LCD_RS;        // RS=0 : Command
  LCD_CMD->BRR = LCD_RW;        // RW=0 : Write
  lcd_nybble();
  if( cmd < 4 ) delay_us( 2000 );
}

// Private Functions

static void lcd_nybble() { 
  char i;
  LCD_CMD->BSRR = LCD_EN; // set bit
  for( i=0; i<5; i++ );   // 2.6us pulse
  LCD_CMD->BRR = LCD_EN;  // reset bit - clock enabled on falling edge
}

static void lcd_lowLevelInit() {
  // enable GPIO for LCD
  RCC_APB2PeriphClockCmd( LCD_CMDCLOCK | LCD_DATACLOCK, ENABLE );

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit( &GPIO_InitStructure );
  GPIO_InitStructure.GPIO_Pin = LCD_RS | LCD_RW | LCD_EN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init( LCD_CMD, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = 0xFF; // P0 - P7
  GPIO_Init( LCD_DATA, &GPIO_InitStructure );
}
