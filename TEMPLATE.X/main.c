/*
 * File:   main.c
 * Author: Lenovo
 *
 * Created on 23 de julio de 2021, 12:57 AM
 */



#define F_CPU 16000000

#include <xc.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*PROTOTIPO DE FUNCIONES*/
void SPI_Init(void);

int main(void) {
    DDRB |= 1U<<DDB5;
    

    ei();
    while(1){
     
        //PINB |= 1<<5;
        PORTB |= 1<<5;
        _delay_ms(50);
        PORTB &=~ 1U<<5;
        //PINB &=~ 1U<<5;
        _delay_ms(50);
        
        
    }
}

void SPI_Init(void){
    
    SPCR |= 1U<<SPIE;           //Interrupt enable
    SPCR |= 1U<<SPE;            //enable SPI
    SPCR &=~ (1U<<MSTR);        //slave mode
    /*CPOL*/
    SPCR |= 1U<<CPOL;
    SPCR |= 1U<<CPHA;
    /*Clock*/
    
}
ISR(SPI_STC_vect)
{
	DDRB = 0;
}

