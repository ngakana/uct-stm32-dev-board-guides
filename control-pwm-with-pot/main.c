/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
LIBRARIES 
--------------------------------------------------------------------------------------------------------------------------------------------
*/ 
#define STM32F051 
#include <stm32f0xx.h>
#include <stm32f051x8.h>
#include <lcd_stm32f0xx.h>

/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
DEFINES 
--------------------------------------------------------------------------------------------------------------------------------------------
*/ 

 
/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
GLOBAL VARIABLES 
------------------------------------------------------------------------------------------------------------------------------------------
*/ 

 

/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
FUNCTIONS DECLARATIONS 
--------------------------------------------------------------------------------------------------------------------------------------------
*/ 
void main(void); 
void init_GPIOA(void); 
void init_ADC(void); 
void adcLCDdisplay(uint8_t number);

/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
MAIN FUNCTION 
--------------------------------------------------------------------------------------------------------------------------------------------
*/ 
void main(void) { 

    init_GPIOA();
    init_ADC();
    lcd_init();

    while(1) {
        ADC1->CR  |=  ADC_CR_ADSTART;  // start conversion
        while( (ADC->ISR & ADC_ISR_EOC) == 0 );  // wait until conversion is complete
        adcLCDdisplay(ADC->DR);  // display conversion result on the LCD
    }
} 

/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
FUNCTIONS DEFINITIONS 
--------------------------------------------------------------------------------------------------------------------------------------------
*/ 
void init_GPIOA(void) { 
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN; // enable clock signal to Port A
    GPIOA -> MODER |= GPIO_MODER_MODER5;  // configure pin PA5 to analog input mode
} 

void init_ADC(void) { 
    ADC1->CR &= ~ADC_CR_ADEN;  // ensure ADC is disabled before calibration
    ADC1->CR |= ADC_CR_ADCAL;  // initiate ADC calibration
    RCC->APB2ENR  |=  RCC_APB2ENR_ADCEN;  // connect ADC to APB clock 
    ADC1->CR  |=  ADC_CR_ADEN;  // enable ADC
    ADC1->CHSELR  |=  ADC_CHSELR_CHSEL5;  // select ADC channel 5(for ADC_IN5 = PA5)
    ADC1->CFGR1  &= ~ADC_CFGR1_CONT;  // set to single conversion mode
    ADC1->CFGR1  |= 0b0000;  // configure ADC to res of 12bits
    while ( (ADC1->ISR & ADC_ISR_ADRDY) == 0 ){ }  // exit when ADC has stabilizeD
}

void adcLCDdisplay(uint8_t number) {
    /* for 12bit res, we expect a decimal number of up to fourdigits */
    uint8_t first_digit = number/1000; 
    uint8_t second_digit = (number –first_digit*1000)/100;
    uint8_t third_digit = (number –second_digit*100–first_digit*1000)/10;
    uint8_t fourth_digit = number –third_digit*10 -second_digit*100 –first_digit*1000;
    
    lcd_command(LCD_CURSOR_HOME);  // reset cursor to home 
    lcd_num( 48 + first_digit ); 
    lcd_num( 48 + second_digit );
    lcd_num( 48 + third_digit );
    lcd_num( 48 + fourth_digit );}
}