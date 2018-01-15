#include <stdio.h>
#include "include/stm32f1xx.h"

void USART1_putchar(char c) {
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

void debug_message(char *data) {
    while(*data) {
        USART1_putchar(*data++);
    }
}

int main(void) {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_USART1EN;

    GPIOC->CRH &= ~(GPIO_CRH_CNF14) | ~(GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE14_1 | GPIO_CRH_MODE13_1;

	GPIOC->ODR |= GPIO_ODR_ODR14 | GPIO_ODR_ODR13;

    /* USART */
    GPIOC->CRH &= ~(GPIO_CRH_CNF9);                     // Clear CNF9
    GPIOA->CRH |= GPIO_CRH_CNF9_1;                      // Alternate Function Push-Pull
    GPIOA->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;  // Output Max Speed 50mhz

    USART1->BRR = 833;                                  // Baud 9600 (8000000/9600 = 833.33)
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE;          // UART Enable, UART TX Enable

    char buffer[100];
    sprintf(buffer, "This string has a number: %d\r\n", 100);

    while(1) {
        debug_message(buffer);
    }
}
