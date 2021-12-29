/*
 * main.c
 *
 * Created: 12/28/2021 7:54:01 PM
 *  Author: Lenovo
 */ 

#define F_CPU 1000000UL

#define UBRR_VALUE(BAUDRATE) (((F_CPU / (BAUDRATE * 8UL))) - 1)

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t buffer[50];
uint16_t len;
uint8_t num;			//numero binario a leerse por (PB3-PB0)

const uint8_t Table7seg[] = {0X3F , 0X06 , 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X67 } ;
	
/*FUNCTION PROTOTYPE*/
void UART_Init(uint16_t baud);
void UART_Transmit(uint8_t data );
void UART_SendData(uint8_t *dataPtr,uint16_t size);
uint8_t UART_Receive( void );
int main(void)
{
	/*PUERTO B COMO SALIDA (display 7seg catodo comum*/
	DDRB = 0xFF;
	/*PC5 INPUT PULL UP*/
	DDRC &=~ (1U<<5);
	PORTC |= 1U<<5;	
	/*UART INIT*/
	UART_Init(4800);
    while(1)
    {
        //Si se recibe un numero
		if(UCSR0A & (1<<RXC0)){
			num = UDR0;
			num = num - 48;
			if(num>= 0 && num<10){
				PORTB = Table7seg[num]; //Se muestra el numero
				_delay_ms(600);
				PORTB = 0;
			}else{
				PORTB = 1U<<7;			//PUNTO DECIMAL
			}
		}
		//Si se presiona
		if(!(PINC & 1U<<5))				//SI SE PRESIONA
		{
			_delay_ms(100);
			for(uint8_t i=0; i<10; i++){
				len = sprintf((char*)buffer,"%d\r\n",i);
				UART_SendData(buffer,len);
				_delay_ms(300);
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

uint8_t UART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
}