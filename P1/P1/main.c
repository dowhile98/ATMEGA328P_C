/*
 * main.c
 *
 * Created: 12/21/2021 3:36:57 PM
 *  Author: Lenovo
 */ 

#define F_CPU 1000000UL  // 1 MHz

#include <xc.h>
#include <util/delay.h>

uint8_t shift;
uint8_t state;

const unsigned int Baudios = 0x000C;
unsigned char dato;

void configura_usart(void);

void Light_Leds(uint8_t ledNumber);
int main(void)
{
	DDRD &=~ _BV(DDD2);					//input
	PORTD |= _BV(PORTD2);				//pull -up resistor
	configura_usart();
	DDRB = 0xFF;
    while(1)
    {
        if(!(PIND & 1U<<PIND2)){
			while(!(PIND & 1U<<PIND2));
			PORTB ^= 1U<<PORTB5;
			state ^= 1;
			PORTB = 1; 
			
			
		}
		if(state == 1){
			Light_Leds(shift);
			_delay_ms(100);
		}
		shift++;
		if(shift>=8)
			shift = 0;
			
		while( !(UCSR0A & (1<<RXC0)) );
		// Lee el dato recibido y analiza si es minúscula o mayúscula
		dato = UDR0;
		if ((dato<='z') && (dato>='a')) dato= dato-0x20; // a mayúscula
		else if ((dato<='Z') && (dato>='A')) dato=dato+0x20; // a minúscula
		// Espera que se libere el buffer de transmisión
		while( !( UCSR0A & (1<<UDRE0)) );
		// Se envía el dato
		UDR0 = dato;
		
    }
}

void Light_Leds(uint8_t ledNumber){
	PORTB = 1U<<ledNumber;
}

void configura_usart(void)
{
	/****** Configuración de parámetros de comunicación *******/
	// 9600 bps
	UBRR0H = (unsigned char)(Baudios>>8);
	UBRR0L = (unsigned char) Baudios;
	//Velocidad doble
	UCSR0A = 1<<U2X0;
	//Comunicación Asíncrona, sin paridad, 1 bit de parada, 8 bits de datos
	UCSR0C=(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
	// Tx y Rx habilitadas
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
}