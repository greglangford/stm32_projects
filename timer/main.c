#include "include/stm32f1xx.h"

int main(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 0x20;                   // /32 prescaler
    TIM2->DIER = TIM_DIER_UIE;          // Enable Update Interrupt
    TIM2->CR1 = TIM_CR1_CEN;            // Timer Enable

    GPIOC->CRH &= ~(GPIO_CRH_CNF14);    // Output Push-Pull
    GPIOC->CRH |= GPIO_CRH_MODE14_1;    // Output mode, 2Mhz

    NVIC_EnableIRQ(TIM2_IRQn);          // Enable Timer2 Interrupt

    while(1) {}
}

void TIM2_IRQHandler(void) {
    TIM2->SR &= ~(TIM_SR_UIF);      // Clear interrupt
    GPIOC->ODR ^= GPIO_ODR_ODR14;   // Invert PC14
}
