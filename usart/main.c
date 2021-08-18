/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
LIBRARIES 
--------------------------------------------------------------------------------------------------------------------------------------------
*/ 
#define STM32F051 
#include <stm32f0xx.h>
#include <stm32f051x8.h>
#include <stdbool.h>

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
uint8_t Data[] = {0, 0}; // array to hold received bytes
uint8_t counter = 0; // keeps count of number of recieved bytes
bool data_received = false; // indicates when all byates have been received

/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
FUNCTIONS DECLARATIONS 
--------------------------------------------------------------------------------------------------------------------------------------------
*/ 
void main(void); 
void init_USART(void); 

/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
MAIN FUNCTION 
--------------------------------------------------------------------------------------------------------------------------------------------
*/ 

void main(void) { 
    init_USART();

    while(1) {
        if ( data_received ) {
            USART1 -> TDR = Data[0] + 1; // increment first word and send back
            while( (USART -> ISR & USART_ISR_TXE) == 0 ); // wait until transimission successful
            USART1 -> TDR = Data[1] + 1;
            while( (USART -> ISR & USART_ISR_TXE) == 0 );

            /* reset variables */
            data_received = false;
            counter = 0;
        }
    } 
} 

/*
-------------------------------------------------------------------------------------------------------------------------------------------- 
FUNCTIONS DEFINITIONS 
--------------------------------------------------------------------------------------------------------------------------------------------
*/ 
void init_USART(void) { 

    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN; // connect clock signal to Port A
    GPIOA -> MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1); // config Tx and Rx pins
    GPIOA -> AFR[1] = ( 0b0001 << (9-8)*4 | 0b0001 << (10-8)*4 ); // enable USART functionality
   
    RCC -> APB2ENR |= RCC_APB2ENR_USRT1EN; // connect USART1 to APB2 clock signal
    USART1 -> BRR = 480000000/9600; // set baud rate to 9600
    USART1 -> CR1 |= USART_CR1_M; // set word length to 1 byte (8-bits)
    USART1 -> CR1 |= ( USART_CR1_PSE | USART_CR1_PS ); // use odd parity checking

    USART1 -> CR1 |= USART_CR1_RXNEIE; // generate interrupt when data is received

    USART1 -> CR1 |= ( USART_CR1_RE | USART_CR1_TE ); // set USART mode to allow receiving and transmitting data
    USART1 -> CR1 |= USART_CR1_UE; // enable USART1

    NVIC_EnableIRQ(USART1_IRQn); // handle data-recieved-interrupt

}  

void NVIC_EnableIRQ(void) { 
    Data[counter] = USART -> RDR; // write recieved word to array

    /* check if all expected data is received  */
    if ( counter >= 1 ) {
        data_received = true;
    }

    counter++; 
}