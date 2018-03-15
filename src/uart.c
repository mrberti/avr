#include "uart.h"
#include "events.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "timer.h"

uart_buffer_t uart_buffer_rx;
uart_buffer_t uart_buffer_tx;

char uart_buffer_rx_data[UART_BUFFER_RX_SIZE];
char uart_buffer_tx_data[UART_BUFFER_TX_SIZE];

static void uart_buffer_init(uart_buffer_t *buf, char *data, uint8_t size);

void UART_init(uint16_t ubrr)
{
	uart_buffer_init(&uart_buffer_rx, uart_buffer_rx_data, UART_BUFFER_RX_SIZE);
	uart_buffer_init(&uart_buffer_tx, uart_buffer_tx_data, UART_BUFFER_TX_SIZE);

	/* set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)(ubrr);

	/* Set frame format: 8data, 0stop bit */
	UCSR0C |= (3<<UCSZ00);
}

void UART_disable(void)
{
	UCSR0B &= ~((1<<TXEN0) | (1<<RXEN0));
}

void UART_enable(void)
{
	UCSR0B |= (1<<TXEN0) | (1<<RXEN0);
	/* Enable interrupts */
	uart_start_listen();
}

void UART_putc(char c)
{
	/* Blocking wait for the UART to be ready*/
	while(uart_is_transmitting());
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

static void uart_buffer_init(uart_buffer_t *buf, char *data, uint8_t size)
{
	buf->data = data;
	buf->index_w = 0;
	buf->index_r = 0;
	buf->index_max = size-1;
	buf->used = 0;
	for(uint8_t i = 0; i < size; i++)
	{
		buf->data[i] = '#';
	}
}

void uart_buffer_write(uart_buffer_t *buf, char c)
{
	/* ATTENTION: No buffer access check is done here!
	The user is responsible to prevent racing conditions!*/
	buf->data[buf->index_w++] = c;
	if(buf->index_w > buf->index_max)
		buf->index_w = 0;
	buf->used++;
}

char uart_buffer_read(uart_buffer_t *buf)
{
	/* ATTENTION: No buffer access check is done here!
	The user is responsible to prevent racing conditions!*/
	char c = buf->data[buf->index_r++];
	if(buf->index_r > buf->index_max)
		buf->index_r = 0;
	buf->used--;
	return c;
}

void uart_buffer_write_int(uart_buffer_t *buf, int i)
{
	char s[7];
	int2str_10(i,s);
	uart_buffer_write_string(buf, s);
}

void uart_buffer_write_long(uart_buffer_t *buf, long i)
{
	char s[12];
	long2str_10(i,s);
	uart_buffer_write_string(buf, s);
}

void uart_buffer_write_string(uart_buffer_t *buf, char *s)
{
	while(*s != 0)
	{
		uart_buffer_write(buf,*s);
		s++;
	}
}

void uart_buffer_dump_data(uart_buffer_t *buf)
{
	UART_puts("\n\r++dump start++\n\r");
	for(uint8_t i = 0; i < buf->index_max+1; i++)
	{
		UART_putc(uart_buffer_tx_data[i]);
	}
	UART_puts("\n\r--dump end--\n\r");
}

ISR(USART_RX_vect)
{
	PORTD ^= LED_UART_RX;
	uart_buffer_write(&uart_buffer_rx, UDR0);
}

ISR(USART_UDRE_vect)
{
	PORTD |= LED_UART_TX;
	if(uart_buffer_tx.used == 0)
	{
		SET_EVF(EVF_UART_BUFFER_TX_FINISHED);
		uart_stop_tx();
	}
	else
	{
		UDR0 = uart_buffer_read(&uart_buffer_tx);
	}
	PORTD &= ~LED_UART_TX;
}

ISR(USART_TX_vect)
{
	PORTD ^= LED_RED;
}

const long divs[] PROGMEM = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};

void int2str_10(int val, char* s)
{
	/*
	TODO: Improvement ideas
	- Put this function into another file
	- There are some black magic tricks where using assembler might shorten the
	conversion from binary to 10 base string much faster
	- The '0' is always aligned left
	*/
	const uint8_t divs_n = 5;
	const uint8_t divs_n_start = 5;
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
	for(uint8_t i = divs_n_start; i < divs_n+divs_n_start; i++)
	{
		div = pgm_read_word(&divs[i]);
		if(div>val && skip_leading_zeros == 1)
		{
			//*s = ' ';
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

void long2str_10(long val, char* s)
{
 		const uint8_t divs_n = 10;
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
			div = pgm_read_dword(&divs[i]);
			if(div>val && skip_leading_zeros == 1)
			{
				//*s = ' ';
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


void uart_buffered_test()
{
	timer0_init();
	timer0_start();
	sei();
	int temp = 0;
	char c = 0;
	while(1)
	{
		if(EVF_IS_SET(EVF_MAIN_LOOP_WAIT_FINISHED))
		{
			uart_buffer_write_string(&uart_buffer_tx, "\n\r");
			uart_buffer_write_long(&uart_buffer_tx, get_us());
			uart_buffer_write_string(&uart_buffer_tx, " ");
			while(uart_buffer_rx.used > 0)
			{
				c = uart_buffer_read(&uart_buffer_rx);
				uart_buffer_write(&uart_buffer_tx, c);
			}
			uart_kickout();
			while(uart_transmit_enabled());
			uart_buffer_write_long(&uart_buffer_tx, get_us());
			CLEAR_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
		}

		temp += 1;
		PORTB ^= (1<<PB5);
	}
}
