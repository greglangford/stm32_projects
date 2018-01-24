#include "include/stm32f1xx.h"

USART_TypeDef *USART;

void USART_putchar(USART_TypeDef *ptrUSART, char c) {
    USART = ptrUSART;

    while(!(USART->SR & USART_SR_TXE));
    USART->DR = c;
}

void debug_message(char *data) {
    while(*data) {
        USART_putchar(USART1, *data++);
    }
}
