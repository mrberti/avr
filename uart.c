#include "uart.h"
#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>

void UART_init(uint16_t ubrr)
{
	/* set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;

	/* Set frame format: 8data, 0stop bit */
	UCSR0C |= (3<<UCSZ00);
}

void UART_disable(void)
{
	UCSR0B &= ~(1<<TXEN0);
}

void UART_enable(void)
{
	UCSR0B |= (1<<TXEN0);
}

void UART_putc(char c)
{
	// Warte auf Sendebereitschaft
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

void UART_puts(char *s)
{
	while(*s)
	{
		UART_putc(*s);
		s++;
	}
}

void UART_putd(int16_t d)
{
	char s[7];
	itoa(d, s, 10);
	UART_puts(s);
}

void UART_puth(int16_t d)
{
	char s[7];
	itoa(d, s, 16);
	UART_puts(s);
}

