#include <stdio.h>
#include <string.h>
#include "include/stm32f1xx.h"

volatile uint8_t isGpsStart = 0;
volatile uint8_t isGpsReady = 0;
char buf[100];
volatile char *ptrbuf;

void USART1_putchar(char c) {
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

char USART2_getchar() {
    while(!(USART2->SR & USART_SR_RXNE));
    return USART2->DR;
}

void debug_message(char *data) {
    while(*data) {
        USART1_putchar(*data++);
    }
}

int main(void) {
    RCC->CR |= RCC_CR_HSEON;                // HSE enable
    while(!(RCC->CR & RCC_CR_HSERDY));      // Wait for HSE ready
    RCC->CFGR &= ~(RCC_CFGR_SW);            // Clear configured clock
    RCC->CFGR |= RCC_CFGR_SW_HSE;           // Configure clock to HSE

    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);  // Wait for HSE clock

    // Enable prepherials
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_USART1EN;

    GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_CNF2);         // CNF1 0, CNF2 0
    GPIOA->CRL |= GPIO_CRL_CNF2_1;                          // A2 AltFunc PP
    GPIOA->CRL |= GPIO_CRL_MODE1_1 | GPIO_CRL_MODE2;        // A1 2mhz, A2 50mhz

    GPIOA->CRH &= ~(GPIO_CRH_CNF9);     // Clear CNF
    GPIOA->CRH |= GPIO_CRH_CNF9_1;      // Alt func PP
    GPIOA->CRH |= GPIO_CRH_MODE9;       // Output 50mhz

    USART1->BRR = 833;                                                              // 9600 baud
    USART1->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;

    USART2->BRR = 833;
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;    // 9600 baud

    debug_message("Starting GPS Tracker ...\r\n");

    ptrbuf = &buf[0];
    memset(buf, 0, 100);

    //NVIC_EnableIRQ(USART2_IRQn);

    GPIOA->ODR |= GPIO_ODR_ODR1;        // Enable GPS

    while(1) {

        // Get GPS String
        while(!isGpsReady) {
            char c;
            c = USART2_getchar();

            if(c == '$') {
                isGpsStart = 1;
            }

            if(c == '\n') {
                isGpsStart = 0;
                isGpsReady = 1;
            }

            *ptrbuf++ = c;
        }

        // Process GPS String
        if(isGpsReady) {
            isGpsReady = 0;

            char *p;
            p = strstr (buf, "$GNGGA");

            if(p) {
                debug_message(buf);
            }

            // Reset pointer and buffer
            ptrbuf = &buf[0];
            memset(buf, 0, 100);
        }
    }
}
