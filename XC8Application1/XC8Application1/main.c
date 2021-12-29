/*
 * main.c
 *
 * Created: 12/28/2021 6:27:10 PM
 *  Author: Lenovo
 */ 
#define F_CPU 1000000UL
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>



int main(void)
{
	/*configurar el pulsador*/
	DDRB &=~ (1U<<DDB0);			//INPUT
	/*configura los pines donde van conectado los leds*/
	DDRB |= 1U<<DDB2 | 1U<<DDB3;	//OUTPUT
	
	for(uint8_t i = 0;i<5;i++){
		while(!(PINB & 1U<<PINB0));	//MIENTRAS NO SE PULSE
		while(PINB & 1U<<PINB0);	//MIENTRAS ESTE PULSADO
	}
	//parpadeo led1
	for(uint8_t i = 0;i<20;i++){
		PORTB ^= 1U<<PORTB2;
		_delay_ms(300);
	}
	PORTB &=~ 1U<<PORTB2;		//se apaga el led1
	//parpadeo led2
	for(uint8_t i = 0;i<20;i++){
		PORTB ^= 1U<<PORTB3;
		_delay_ms(300);
	}
	//encender los dos diodos
	PORTB |= 1U<<PORTB3 | 1U<<PORTB2;
    while(1)
    {
        //TODO:: Please write your application code 
    }
}