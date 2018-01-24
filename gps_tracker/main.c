#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/stm32f1xx.h"
#include "include/uart.h"
#include "include/gps.h"

char buf[GPS_STRING_BUFFER_LEN];

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
    GPIOA->ODR |= GPIO_ODR_ODR1;        // Enable GPS

    while(1) {
        if(gps_get_sentence(USART2, "$GNRMC", buf)) {
            debug_message(buf);
        }
    }
}
