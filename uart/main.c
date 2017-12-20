#include "include/stm32f1xx.h"

// Send char from USART1
void USART1_putchar(char c) {
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

// Send message to console output
void debug_message(char *data) {
    while(*data) {
        USART1_putchar(*data++);
    }
}

int main(void) {
    // Alternate Function Clock Enable, PORTA Enable, USART1 Enable
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    /* USART */
    GPIOA->CRH |= GPIO_CRH_CNF9_1;                      // Alternate Function Push-Pull
    GPIOA->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;  // Output Max Speed 50mhz

    USART1->BRR = 833;                                  // Baud 9600 (8000000/9600 = 833.33)
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE;          // UART Enable, UART TX Enable

    while(1) {
        debug_message("This is a big long string\r\n");
    }
}
