#include "global.h"
#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{

	//UART_main_test();

	DDRB = (1<<PB5);
	DDRD = (1<<PD3);
	while(1)
	{
		PORTD ^= (1<<PD3);
		PORTB ^= (1<<PB5);
		_delay_ms(500);
	}

	return 0;
}
