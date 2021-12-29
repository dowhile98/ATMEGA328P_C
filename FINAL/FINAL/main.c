/*
 * main.c
 *
 * Created: 12/21/2021 6:19:35 PM
 *  Author: Lenovo
 */ 

#define F_CPU 1000000UL

#include <stdio.h>
#include <string.h>
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>

uint8_t dataRx;
uint8_t duty = 40;
uint8_t buffer[100];
uint16_t len;


#define UBRR_VALUE(BAUDRATE) (((F_CPU / (BAUDRATE * 8UL))) - 1)

void UART_Init(uint16_t baud);

void UART_Transmit(uint8_t data );

uint8_t UART_Receive( void );

void UART_SendData(uint8_t *dataPtr,uint16_t size);

void TIMER1_ConfiPWM(void);

uint16_t setDuty(uint8_t duty);
int main(void)
{
	/*button init*/
	DDRC &=~ (1U<<DDC1);  //PC1
	PORTC |= 1U<<PORTC1;	//PULL UP
	//UART INIT
	UART_Init(9600);
	//PWM INIT
	TIMER1_ConfiPWM();
	len = sprintf((char*)buffer,"configuracion correcta\r\n");
	
	UART_SendData(buffer,len);
	
    while(1)
    {
		//decremento
		if(!(PINC & 1U<<PINC1)){ //SI SE PRESIONA EL PULSADOR
			_delay_ms(100);		 //para el rebote
			duty -=10;				//decrementa en 10%
			if(duty<10){ //se verifca el rango
				duty  = 10;	
			}
			OCR1A  = setDuty(duty);
		}
		//incremento
		if(UCSR0A & (1<<RXC0)){
			dataRx = UDR0;
			UART_Transmit(dataRx);
			if(dataRx == 'A'){//se verifica el dato recibido
				duty += 10;
				if(duty>90){ //se verifca el rango
					duty = 90;				//decrementa en 10%
				}
				OCR1A  = setDuty(duty);
			} 
		}
		_delay_ms(200);						//retardo
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


void TIMER1_ConfiPWM(void){
	 DDRB |= ( 1<< 1 | 1U<<2);  // Configuramos el PB1 como salida.
	 TCNT1 = 0; // Reiniciamos el contador inicial (por siacaso)
	 ICR1 = 6249; // Configuramos el periodo de la señal (el TOP de nuestra PWM)
	 TCCR1A =  (1 << COM1A1) | (0 << COM1A0) ; // Ponemos a 'bajo' el OCR1A cuando coincida el Compare Match
	 TCCR1A	|= 1U<<COM1B1 | 0<<COM1B0;
	 TCCR1A |=  (1 << WGM11) | (0 << WGM10) ; // Fast PWM: TOP: ICR1
	 TCCR1B = (1 << WGM13) | (1 << WGM12); // // Fast PWM: TOP: ICR1
	 TCCR1B |= (0 << CS12) | (1 << CS11) | ( 0 << CS10 ); // Preesc = 8
	 OCR1A = setDuty(duty); // duty 40%
	 OCR1B = setDuty(50);
	return;
}

uint16_t setDuty(uint8_t duty){
	uint16_t temp = duty * (6249/100);
	return temp;
}