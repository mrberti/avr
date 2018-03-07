#include "global.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "buffer.h"

#include <avr/io.h>
#include <util/delay.h>

buffer_u16_t buf;

int main(void)
{
	DDRB = (1<<PB5);
	DDRD = (1<<PD3) | (1<<PD2);

	UART_init(UART_UBRR_500k);
	UART_enable();
	UART_puts("\n\r\n\rUART_initialized.\n\r\n\r");

	timer0_init();
	timer0_start();

	//ADC_init();

	buffer_u16_init(&buf);

	int i = 0;
	//char c = 0;
	while(1)
	{
		i += 10;
		uint8_t usage = 0;
		UART_putd(i);
		UART_puts(",");
		UART_putd(buffer_u16_write(&buf, i));
		UART_puts(",");
		UART_putd(buffer_u16_usage(&buf, &usage));
		UART_puts(",usage ");
		UART_putd(usage);
		if(usage == BUFFER_SIZE_MAX)
		{
			UART_puts(", values: ");
			while(usage > 0)
			{
				uint16_t val = 0;
				buffer_u16_read(&buf,&val);
				buffer_u16_usage(&buf, &usage);
				UART_putd(val);
				UART_puts(", ");
				PORTD |= (1<<PD3);
			}
		}
		PORTD &= ~(1<<PD3);
		UART_puts("\n\r");
		timer0_wait_ms_blocking(100);
	}
	return 0;
}
