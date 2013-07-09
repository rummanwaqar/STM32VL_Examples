#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_exti.h>
#include <misc.h>

void spi_init( void );
void Delay(uint32_t nTimes);

int ledVal;

int main(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  if (SysTick_Config(SystemCoreClock / 1000))
    while (1);

  ledVal = 0;
  spi_init();

  while(1) {
    static int ledVal1 = 0;
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, (ledVal1) ? Bit_SET : Bit_RESET);
    ledVal1 = 1 - ledVal1;
  }
}

void EXTI4_IRQHandler( void ) {
  if( EXTI_GetITStatus( EXTI_Line4 ) != RESET ) {
    EXTI_ClearFlag( EXTI_Line4 );
    SPI_I2S_SendData( SPI1, 0x45 );
    while( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_RXNE ) == RESET );
    if( SPI_I2S_ReceiveData( SPI1 ) == 0xAA ) {
      GPIO_WriteBit( GPIOC, GPIO_Pin_8, (ledVal) ? Bit_SET : Bit_RESET );
      ledVal = 1 - ledVal;
    }
  }
}

void spi_init() {
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1 | \
                          RCC_APB2Periph_GPIOA | \ 
                          RCC_APB2Periph_AFIO, ENABLE );

  // SS Config
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOA, &GPIO_InitStructure );
  
  GPIO_EXTILineConfig( GPIO_PortSourceGPIOA, GPIO_PinSource4 );
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init( &EXTI_InitStructure );

  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOA, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init( GPIOA, &GPIO_InitStructure );
 
  // setup spi
  SPI_I2S_DeInit( SPI1 );
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 1;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_Init( SPI1, &SPI_InitStructure );
  SPI_Cmd( SPI1, ENABLE );
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
