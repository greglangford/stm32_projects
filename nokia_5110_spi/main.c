#include "include/stm32f1xx.h"
#include "include/pcd8544.h"

/*
Screen Configuration
-----------------
PA2 -> RESET
PA3 -> CE
PA0 -> DC
PA7 -> MOSI
PA5 -> CLK
*/

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

char spi_transfer(char c) {
    while(!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = c;
    //while(!(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR;
}

void lcd_reset() {
    // Configure timer
    TIM2->PSC = 0x00;   // 8mhz
    TIM2->ARR = 8;     // 2us

    // Generate pulse
    GPIOA->ODR &= ~(GPIO_ODR_ODR2);
    TIM2->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;
    while((TIM2->CR1 & TIM_CR1_CEN));
    GPIOA->ODR |= GPIO_ODR_ODR2;
}

void lcd_command(char c) {
    GPIOA->ODR &= ~(GPIO_ODR_ODR3); // CE
    GPIOA->ODR &= ~(GPIO_ODR_ODR0); // DC
    spi_transfer(c);
    GPIOA->ODR |= GPIO_ODR_ODR3;    // CE
}

void lcd_data(char c) {
    GPIOA->ODR &= ~(GPIO_ODR_ODR3); // CE
    GPIOA->ODR |= GPIO_ODR_ODR0;    // DC
    spi_transfer(c);
    GPIOA->ODR |= GPIO_ODR_ODR3;    // CE
}

void lcd_begin() {
    lcd_reset();

    lcd_command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
    lcd_command(PCD8544_SETBIAS | 0x04);
    lcd_command(PCD8544_SETVOP | 0x7f);
    lcd_command(PCD8544_FUNCTIONSET);
    lcd_command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

    lcd_command(PCD8544_SETXADDR | 2);  // x position
    lcd_command(PCD8544_SETYADDR);  // x position
}

int main(void) {
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));

    // Alternate Function Clock Enable, PORTA Enable, USART1 Enable
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_SPI1EN;
    // Timer 2 Enable
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Clear CNF
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 |GPIO_CRL_CNF2 | GPIO_CRL_CNF3 | GPIO_CRL_CNF5 | GPIO_CRL_CNF7);
    // PA5 AF PP, PA7 AF PP
    GPIOA->CRL |= GPIO_CRL_MODE5_1 | GPIO_CRL_MODE7_1;
    // PA0 Out 2Mhz, PA2 Out 2Mhz, PA3 Out 2Mhz, PA5 Out 50mhz, PA7 Out 50mhz, PA5 AF PP, PA7 AF PP
    GPIOA->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE2_1 | GPIO_CRL_MODE3_1 | GPIO_CRL_MODE5 | GPIO_CRL_MODE7 | GPIO_CRL_CNF5_1 | GPIO_CRL_CNF7_1;
    // Clear CNF
    GPIOA->CRH &= ~(GPIO_CRH_CNF9);
    // PA9 AF PP, PA9 Out 50Mhz
    GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9;

    // USART Configuration
    USART1->BRR = 833;                                  // Baud 9600 (8000000/9600 = 833.33)
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE;          // UART Enable, UART TX Enable

    // SPI
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SPE;

    // Output High
    GPIOA->ODR = GPIO_ODR_ODR0 | GPIO_ODR_ODR2 | GPIO_ODR_ODR3;

    debug_message("Hello world\r\n");


    //lcd_begin();

    //lcd_command(PCD8544_SETXADDR);  // x position
    //lcd_command(PCD8544_SETYADDR);  // x position

    GPIOA->ODR &= ~(GPIO_ODR_ODR3); // CE
    GPIOA->ODR |= GPIO_ODR_ODR0;    // DC

    spi_transfer(0x3e);
    spi_transfer(0x41);
    spi_transfer(0x49);
    spi_transfer(0x49);
    spi_transfer(0x7a);
    spi_transfer(0x00);

    spi_transfer(0x7f);
    spi_transfer(0x09);
    spi_transfer(0x19);
    spi_transfer(0x29);
    spi_transfer(0x46);
    spi_transfer(0x00);

    spi_transfer(0x7f);
    spi_transfer(0x49);
    spi_transfer(0x49);
    spi_transfer(0x49);
    spi_transfer(0x41);
    spi_transfer(0x00);

    spi_transfer(0x3e);
    spi_transfer(0x41);
    spi_transfer(0x49);
    spi_transfer(0x49);
    spi_transfer(0x7a);

    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x00);

    spi_transfer(0x7f);
    spi_transfer(0x09);
    spi_transfer(0x19);
    spi_transfer(0x29);
    spi_transfer(0x46);
    spi_transfer(0x00);

    spi_transfer(0x3f);
    spi_transfer(0x40);
    spi_transfer(0x40);
    spi_transfer(0x40);
    spi_transfer(0x3f);
    spi_transfer(0x00);

    spi_transfer(0x7f);
    spi_transfer(0x40);
    spi_transfer(0x40);
    spi_transfer(0x40);
    spi_transfer(0x40);
    spi_transfer(0x00);

    spi_transfer(0x7f);
    spi_transfer(0x49);
    spi_transfer(0x49);
    spi_transfer(0x49);
    spi_transfer(0x41);
    spi_transfer(0x00);

    spi_transfer(0x61);
    spi_transfer(0x51);
    spi_transfer(0x49);
    spi_transfer(0x45);
    spi_transfer(0x43);
    spi_transfer(0x00);

    GPIOA->ODR |= GPIO_ODR_ODR3; // CE

    while(1) {
    }
}
