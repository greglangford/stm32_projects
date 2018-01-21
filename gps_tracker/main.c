#include <stdio.h>
#include <string.h>
#include "include/stm32f1xx.h"

volatile uint8_t gps_start = 0;
volatile uint8_t gps_ready = 0;
volatile int bufpos = 0;
char buf[100];

void USART1_putchar(char c) {
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

void USART2_putchar(char c) {
    while(!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

void debug_message(char *data) {
    while(*data) {
        USART1_putchar(*data++);
    }
}

void debug_message_usart2(char *data) {
    while(*data) {
        USART2_putchar(*data++);
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

    GPIOA->CRL &= ~(GPIO_CRL_CNF2);
    GPIOA->CRL |= GPIO_CRL_CNF2_1;      // Alt func PP
    GPIOA->CRL |= GPIO_CRL_MODE2;

    GPIOA->CRH &= ~(GPIO_CRH_CNF9);     // Clear CNF
    GPIOA->CRH |= GPIO_CRH_CNF9_1;      // Alt func PP
    GPIOA->CRH |= GPIO_CRH_MODE9;       // Output 50mhz

    USART1->BRR = 833;
    USART1->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;

    USART2->BRR = 833;
    USART2->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;

    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART2_IRQn);

    debug_message("Starting\r\n");

    while(1) {
        if(gps_ready == 1) {
            gps_ready = 0;

            char *p;
            p = strstr(buf, "$GNGGA");

            if(p) {
                debug_message_usart2(buf);
                debug_message_usart2("\r\n");
            }

            // Reset GPS
            memset(buf, 0, 100);
            bufpos = 0;
            gps_start = 0;
            USART1->CR1 |= USART_CR1_RXNEIE;
        }
    }
}

void USART1_IRQHandler(void) {
    char c;
    if((USART1->SR & USART_SR_RXNE)) {
        c = USART1->DR;

        if(c == '$') {
            gps_start = 1;
        }

        if(gps_start == 1) {
            buf[bufpos] = c;
            bufpos++;
        }

        if(c == '\r') {
            USART1->CR1 &= ~(USART_CR1_RXNEIE);
            gps_ready = 1;
        }
    }
}
