#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

// init usart and gpio pins
int uart_open(USART_TypeDef *USARTx, uint32_t baud) {
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructureTx;
  GPIO_InitTypeDef GPIO_InitStructureRx;

  assert_param(IS_USART_123_PERIPH(USARTx));

  if (USARTx == USART1) {
    // turn on clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | \
                           RCC_APB2Periph_AFIO | \
                           RCC_APB2Periph_USART1, ENABLE);

    // Configure Tx Pin
    GPIO_StructInit(&GPIO_InitStructureTx);
    GPIO_InitStructureTx.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructureTx.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructureTx.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructureTx);

    // Configure Rx Pin
    GPIO_StructInit(&GPIO_InitStructureRx);
    GPIO_InitStructureRx.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructureRx.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructureRx);

    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    return 0;
  }
}

int uart_putc(char c, USART_TypeDef* USARTx) {
  assert_param(IS_USART_123_PERIPH(USARTx));

  while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
  USARTx->DR = c;
  return 0;
}

char uart_getc(USART_TypeDef* USARTx) {
  assert_param(IS_USART_123_PERIPH(USARTx));

  while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
  return USARTx->DR & 0xff;
}

int uart_puts(char s[], USART_TypeDef* USARTx) {
  int i = 0;
  while (s[i] != 0x00) {
    uart_putc(s[i], USARTx);
    i++;
  }  
}
