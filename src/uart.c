#include "uart.h"
#include "buffer.h"

#include <inttypes.h>
#include <avr/io.h>
//#include <stdlib.h>
#include <avr/interrupt.h>

BUFFER_DECLARE(buf_uart_rx,char,UART_BUF_RX_SIZE);
BUFFER_DECLARE(buf_uart_tx,char,UART_BUF_TX_SIZE);

void UART_init(uint16_t ubrr)
{
	BUFFER_INIT(buf_uart_rx,char,UART_BUF_RX_SIZE);
	BUFFER_INIT(buf_uart_tx,char,UART_BUF_TX_SIZE);

	/* set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)(ubrr);

	/* Set frame format: 8data, 0stop bit */
	UCSR0C |= (3<<UCSZ00);

	/* Enable interrupts */
	UCSR0B = (1<<RXCIE0);
}

void UART_disable(void)
{
	UCSR0B &= ~(1<<TXEN0);
}

void UART_enable(void)
{
	UCSR0B |= (1<<TXEN0) | (1<<RXEN0);
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
	int2str_10(d, s);
	UART_puts(s);
}

void UART_putd_32(int32_t val)
{
	char s[11];
	long2str_10(val, s);
	//ltoa(val, s, 10);
	UART_puts(s);
}
/*
void UART_puth(int16_t d)
{
	char s[7];
	itoa(d, s, 16);
	UART_puts(s);
}
*/

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

void uart_kickout()
{
	UCSR0B |= (1<<UDRIE0);
}

void long2str_10(long val, char* s)
{
 		const uint8_t divs_n = 10;
		long divs[] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};
		long div;
		uint8_t skip_leading_zeros = 1;

		if(val == 0)
		{
			*s = '0';
			*(s+1) = 0;
			return;
		}
		if(val < 0)
		{
			val = -val;
			*s = '-';
			s++;
		}
		for(uint8_t i = 0; i < divs_n; i++)
		{
			div = divs[i];
			if(div>val && skip_leading_zeros == 1)
			{
				//*s = '';
				//s++;
				continue;
			}
			skip_leading_zeros = 0;
			*s = '0';
			while(val>=div)
			{
				val -= div;
				*s += 1;
			}
			s++;
		}
		*s = 0;
}

void int2str_10(int val, char* s)
{
 		const uint8_t divs_n = 5;
		int divs[] = {10000, 1000, 100, 10, 1};
		int div;
		uint8_t skip_leading_zeros = 1;

		if(val == 0)
		{
			*s = '0';
			*(s+1) = 0;
			return;
		}
		if(val < 0)
		{
			val = -val;
			*s = '-';
			s++;
		}
		for(uint8_t i = 0; i < divs_n; i++)
		{
			div = divs[i];
			if(div>val && skip_leading_zeros == 1)
			{
				//*s = '';
				//s++;
				continue;
			}
			skip_leading_zeros = 0;
			*s = '0';
			while(val>=div)
			{
				val -= div;
				*s += 1;
			}
			s++;
		}
		*s = 0;
}

void uart_buffer_write_int(int i)
{
	char s[7];
	int2str_10(i,s);
	uart_buffer_write_string(s);
}

void uart_buffer_write_long(long i)
{
	char s[12];
	long2str_10(i,s);
	uart_buffer_write_string(s);
}

void uart_buffer_write_string(char *s)
{
	/* TODO: Implement buffer error checking */
	while(*s != 0)
	{
		if(buffer_write(&buf_uart_tx,s) == BUFFER_FULL)
			return;
		s++;
	}
}

ISR(USART_RX_vect)
{
	PORTD ^= LED_GREEN;
	char udr0 = UDR0;
	buffer_write(&buf_uart_rx,&udr0);
}

ISR(USART_UDRE_vect)
{
	PORTD |= LED_YELLOW;
	char rx_out;
	buffer_result_t buf_result;
	buf_result = buffer_read(&buf_uart_tx,&rx_out);
	switch(buf_result){
		case BUFFER_EMPTY:
			UCSR0B &= ~(1<<UDRIE0);
			break;
		case BUFFER_LOCKED:
			break;
		case BUFFER_SUCCESS:
			UDR0 = rx_out;
			break;
	}
	PORTD &= ~LED_YELLOW;
}

ISR(USART_TX_vect)
{
	PORTD ^= LED_RED;
}
