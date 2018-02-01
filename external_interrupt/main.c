#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/stm32f1xx.h"
#include "include/main.h"

void USART_putstring(char *buf) {
    while(*buf) {
        while(!(USART1->SR & USART_SR_TXE));
        USART1->DR = *buf++;
    }
}

int main(void) {
    RCC->CR |= RCC_CR_HSEON;                                // HSE enable
    while(!(RCC->CR & RCC_CR_HSERDY));                      // Wait for HSE ready
    RCC->CFGR &= ~(RCC_CFGR_SW);                            // Clear configured clock
    RCC->CFGR |= RCC_CFGR_SW_HSE;                           // Configure clock to HSE
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);  // Wait for HSE clock

    // Enable prepherials
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    // A0 Floating Input
    GPIOA->CRL &= ~(GPIO_CRL_CNF0);         // Clear
    GPIOA->CRL |= GPIO_CRL_CNF0_0;          // A0 Input PU PD

    GPIOA->CRH &= ~(GPIO_CRH_CNF9   | GPIO_CRH_MODE10);     // Clear
    GPIOA->CRH |= GPIO_CRH_CNF9_1   | GPIO_CRH_CNF10_1;     // A9, A10 AF PP
    GPIOA->CRH |= GPIO_CRH_MODE9    | GPIO_CRH_MODE10;      // A9, A10 50mhz

    USART1->BRR = 833;
    USART1->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;

    GPIOA->ODR |= GPIO_ODR_ODR0;

    // A0 used as interrupt
    EXTI->IMR |= EXTI_IMR_MR0;              // A0 Interrupt Enable
    EXTI->EMR |= EXTI_EMR_MR0;              // A0 Event Request Enable
    EXTI->RTSR |= EXTI_RTSR_TR0;            // Rising Edge Detection
    EXTI->FTSR |= EXTI_FTSR_TR0;            // Falling Edge Detection
    AFIO->EXTICR[1] |= AFIO_EXTICR1_EXTI0;  // Alternate Function enable EXTI Interrupt

    // Enable EXTI0 Global Interrupt
    NVIC_EnableIRQ(EXTI0_IRQn);

    while(1) {

    }
}

// Handle EXTI0 Interrupt
void EXTI0_IRQHandler(void) {
    if((EXTI->PR & EXTI_PR_PR0)) {
        EXTI->PR |= EXTI_PR_PR0;
        USART_putstring("Interrupt\r\n");
    }
}
