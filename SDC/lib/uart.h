#ifndef _UART_H
#define _UART_H

int uart_open(USART_TypeDef *USARTx, uint32_t baud);
int uart_putc(char c, USART_TypeDef* USARTx);
char uart_getc(USART_TypeDef *USARTx);
int uart_puts(char s[], USART_TypeDef* USARTx);

#endif
