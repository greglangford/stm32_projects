#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/stm32f1xx.h"
#include "include/main.h"

char buf[100];
volatile int gpio_reg;
volatile int end_segment;
volatile int capture_buf_pos;
int capture_buf[50];
volatile int preamble;
volatile int timer_capture;
volatile uint8_t timer_capture_flag;

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

    // A3 Floating Input
    GPIOA->CRL &= ~(GPIO_CRL_CNF3);         // Clear
    GPIOA->CRL |= GPIO_CRL_CNF3_0;          // A3 Input PU PD

    GPIOA->CRH &= ~(GPIO_CRH_CNF9   | GPIO_CRH_MODE10);     // Clear
    GPIOA->CRH |= GPIO_CRH_CNF9_1   | GPIO_CRH_CNF10_1;     // A9, A10 AF PP
    GPIOA->CRH |= GPIO_CRH_MODE9    | GPIO_CRH_MODE10;      // A9, A10 50mhz

    USART1->BRR = 833;
    USART1->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;

    USART_putstring("Welcome to Timer 2 Input Capture ...\r\n");

    end_segment = 0;
    capture_buf_pos = 0;
    timer_capture = 0;
    timer_capture_flag = 0;

    TIM2->PSC = 8;
    TIM2->DIER |= TIM_DIER_CC4IE;
    TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
    TIM2->CCER |= TIM_CCER_CC4E;
    TIM2->CR1 |= TIM_CR1_CEN;

    // Enable EXTI0 Global Interrupt
    NVIC_EnableIRQ(TIM2_IRQn);

    while(1) {
        if(capture_buf_pos >= 10) {
            capture_buf_pos = 0;
            preamble = 0;
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Code: %d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\r\n", capture_buf[0], capture_buf[1], capture_buf[2], capture_buf[3], capture_buf[4], capture_buf[5], capture_buf[6], capture_buf[7]);
            USART_putstring(buf);
        }
    }
}

// Handle EXTI0 Interrupt
void TIM2_IRQHandler(void) {
    gpio_reg = GPIOA->IDR;

    if((TIM2->SR & TIM_SR_CC4IF)) {
        timer_capture = TIM2->CCR4;
        gpio_reg = GPIOA->IDR;

        if(gpio_reg & GPIO_IDR_IDR3) {
            // Rising Edge
            if(preamble) {
                if((timer_capture >= 200) && (timer_capture <= 1500)) {
                    capture_buf[capture_buf_pos] = timer_capture;
                    capture_buf_pos++;
                } else {
                    preamble = 0;
                }
            } else {
                if((timer_capture >= 5000) && (timer_capture <= 10000)) {
                    preamble = 1;
                }
            }



            TIM2->CCER |= TIM_CCER_CC4P;
        } else {
            // Falling Edge
            TIM2->CCER &= ~TIM_CCER_CC4P;
        }
    }

    TIM2->CNT = 0;
}
