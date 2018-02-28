#include "global.h"
#include "uart.h"
#include "timer.h"

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB = (1<<PB5);
	DDRD = (1<<PD3) | (1<<PD2);

	UART_init(UART_UBRR_500k);
	UART_enable();

	timer0_init();
	timer0_start();

	//uint8_t i = 0;
	while(1)
	{
		// UART_putd_32(timer0_ovfl_interrupt_call_counter);
		// UART_puts("\t");
		// UART_putd_32(timer0_compa_interrupt_call_counter);
		// UART_puts("\t");
		// UART_putd_32(timer0_compb_interrupt_call_counter);
		// UART_puts("\t");
		UART_putd_32(timer0_us_since_start);
		UART_puts("\t");
		UART_putd_32(timer0_ms_since_start);
		UART_puts("\n\r");
		timer0_wait_ms_blocking(1000);
		//PORTD ^= (1<<PD2);
		//i = i+10;
		//timer0_set_ocrb(i);
	}
	return 0;
}
