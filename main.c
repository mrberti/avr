#include "global.h"
#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

int main(void)
{
	UART_init(UBRR_VAL);
	UART_enable();
	UART_puts("\033[32m");
	while(1)
	{
		UART_puts("\033[33m");
		UART_putd(UBRR0H);
		UART_puts(" ");
		UART_putd(UBRR0L);
		UART_puts(" ");
		UART_putd(UBRR_VAL);
		UART_puts("\n\r");
		_delay_ms(50);
		//UART_puts("\033[2J");
	}
	return 0;
}
