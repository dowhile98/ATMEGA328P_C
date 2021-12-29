/*
 * main.c
 *
 * Created: 12/28/2021 7:03:26 PM
 *  Author: Lenovo
 */ 

#define F_CPU 1000000UL

#include <stdio.h>
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define UBRR_VALUE(BAUDRATE) (((F_CPU / (BAUDRATE * 8UL))) - 1)

/*GLOBAL VARIABLES*/
uint8_t buffer[50];
uint16_t len;
uint8_t num;			//numero binario a leerse por (PB3-PB0)
/*FUNCTION PROTOTYPE*/
void UART_Init(uint16_t baud);
void UART_Transmit(uint8_t data );
void UART_SendData(uint8_t *dataPtr,uint16_t size);

int main(void)
{
	/*SE CONFIGURA PB3-PB0 -> INPUT PULL-UP*/
	DDRB &=~ (1U<<DDB3 | 1U<<DDB2 | 1U<<DDB1 | 1U<<DDB0);
	PORTB |= 1U<<PORTB3 | 1U<<PORTB2 | 1U<<PORTB1 | 1U<<PORTB0;
	/*SE CONFIGURA PC4 (P0)*/
	DDRC &=~ (1U<<DDC4);
	PORTC |= 1U<<PORTC4;	//PULL UP
	//SE CONFIGURA EL UART
	UART_Init(4800);
    while(1)
    {
        //si se presiona el P0 (validar)
		if(!(PINC & 1U<<4)){
			while(!(PINC & 1U<<4))//mientras se mantenga presionado
			//se lee el numero
			num = 0xF & PINB;
			if(num<9){//si el numero es menor a 9
				len = sprintf((char*)buffer,"1523210219\r\n");
				UART_SendData(buffer,len);
			}
			else{//si el numero es mayor o igual a 9
				len = sprintf((char*)buffer,"Gonzales\r\n");
				UART_SendData(buffer,len); //se envia por el puerto serial
			}
		}
			
    }
}

void UART_Init(uint16_t baud){
	baud = UBRR_VALUE(baud);
	/*set baudrate*/
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t)baud;
	/*double speed*/
	UCSR0A = 1U<<U2X0;
	/*Enable receiver and transmitter*/
	UCSR0B |= 1U<<RXEN0 | 1U<<TXEN0;
	/*Set communication parameters*/
	UCSR0C |= 1U<<UCSZ01 | 1U<<UCSZ00;	//8-bit de datos
	UCSR0C |= 0U<<USBS0;				//1-bit stop
	UCSR0C |= 0U<<UPM01 | 0U<<UPM00;    //no parity
}

void UART_Transmit(uint8_t data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
void UART_SendData(uint8_t *dataPtr,uint16_t size){
	while(size>0){
		UART_Transmit(*dataPtr);
		dataPtr++;
		size--;
	}
}