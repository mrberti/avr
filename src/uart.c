#include "uart.h"

#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>

void UART_init(uint16_t ubrr)
{
	/* set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)(ubrr);

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
	/* Blocking wait for the UART to be ready*/
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

void UART_putd_32(int32_t val)
{
	char s[11];
	ltoa(val, s, 10);
	UART_puts(s);
}

void UART_puth(int16_t d)
{
	char s[7];
	itoa(d, s, 16);
	UART_puts(s);
}

void UART_clear_screen()
{
	UART_puts("\033[2J");
}

void UART_print_val(char *str_baud, uint16_t ubrr, uint32_t baud_real)
{
	UART_puts("\n\r");
	UART_puts(str_baud);
	UART_puts("\t");
	UART_putd_32(baud_real);
	UART_puts("\t");
	UART_putd(ubrr);
}

void UART_print_ubrr_vals()
{
	UART_puts("\033[2J");
	UART_puts("Baud rates:\n\r\n\r");
	UART_puts("BAUD\tREAL\tUBRR");

	UART_print_val("9600   ", UART_UBRR_9k6, UART_BAUD_REAL_9k6);
	UART_print_val("14400  ", UART_UBRR_14k4, UART_BAUD_REAL_14k4);
	UART_print_val("19200  ", UART_UBRR_19k2, UART_BAUD_REAL_19k2);
	UART_print_val("28800  ", UART_UBRR_28k8, UART_BAUD_REAL_28k8);
	UART_print_val("38400  ", UART_UBRR_38k4, UART_BAUD_REAL_38k4);
	UART_print_val("57600  ", UART_UBRR_57k6, UART_BAUD_REAL_57k6);
	UART_print_val("76800  ", UART_UBRR_76k8, UART_BAUD_REAL_76k8);
	UART_print_val("115200 ", UART_UBRR_115k2, UART_BAUD_REAL_115k2);
	UART_print_val("230400 ", UART_UBRR_230k4, UART_BAUD_REAL_230k4);
	UART_print_val("250000 ", UART_UBRR_250k, UART_BAUD_REAL_250k);
	UART_print_val("500000 ", UART_UBRR_500k, UART_BAUD_REAL_500k);
	UART_print_val("1000000", UART_UBRR_1000k, UART_BAUD_REAL_1000k);

	UART_puts("\n\r");
}

void UART_main_test()
{
	UART_init(UART_UBRR_500k);
	UART_enable();
	UART_print_ubrr_vals();
}
