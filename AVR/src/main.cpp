#include <Arduino.h>
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
void setup() {
  DDRB = 0xFF;
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  PORTB = 0x00;
  _delay_ms(500);
  PORTB = 0x20;
  _delay_ms(500);
}
