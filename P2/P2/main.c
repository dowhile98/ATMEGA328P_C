/*
 * main.c
 *
 * Created: 12/21/2021 7:45:54 PM
 *  Author: Lenovo
 */ 
#define F_CPU 1000000UL

#define UBRR_VALUE(BAUDRATE) (((F_CPU / (BAUDRATE * 8UL))) - 1)

#include<stdio.h>
#include <xc.h>

void UART_Init(uint16_t baud);
void TxDigit(uint16_t num);


int main(void)
{
	UART_Init(9600);
	TxDigit(123);
    while(1)
    {
        //TODO:: Please write your application code 
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

uint8_t UART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
}

void UART_SendData(uint8_t *dataPtr,uint16_t size){
	while(size>0){
		UART_Transmit(*dataPtr);
		dataPtr++;
		size--;
	}
}
void TxDigit(uint16_t num){
	uint8_t buffer[3] = {'0','0','0'};
	
	sprintf((char*)buffer,"%d",num);
	
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = buffer[0];
	
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = buffer[1];
	
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = buffer[2];
}