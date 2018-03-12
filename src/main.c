#include "global.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "buffer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t event_flags;

uint32_t us_n1 = 0;
uint32_t us_turnaround = 0;
uint32_t us_delta = 0;
uint32_t us_start = 0;

static void main_loop1(void);
static void main_loop2(void);
static void buffer_test1(void);
static void buffer_test2(void);
static void uart_buffered_test(void);

int main(void)
{
	DDRB = (1<<PB5);
	DDRD = (1<<PD4) | (1<<PD3) | (1<<PD2);

	UART_init(UART_UBRR_500k);
	UART_enable();
	UART_clear_screen();
	UART_puts("\n\r\
UART_initialized\n\r\
\n\r");

	uart_buffered_test();
	buffer_test2();
	buffer_test1();
	main_loop1();
	main_loop2();

	/* Never reached */
	return 0;
}

void main_loop1()
{
	timer0_init();
	timer0_start();
	ADC_init();


	#define ADCs 3
	uint8_t adc_chan = 0;
	ADC_val_t vals[ADCs];
	//ADC_val_t val;

	while(1)
	{
		/* Idle wait */
		while(event_flags == 0) {
			PORTD &= ~(1<<PD2);
		}
		PORTD |= (1<<PD2);

		if(EVF_IS_SET(EVF_START_ADC))
		{
			/* Start new conversion cycle */
			adc_chan = 0;
			ADC_start_conversion(adc_chan);

			/* Send out all gathered ADC values */
			UART_putd_32(timer0_us_since_start);
			UART_putc('\t');
			for(uint8_t i = 0; i<ADCs; i++)
			{
				UART_putd(vals[i]);
				UART_putc('\t');
			}
			UART_puts("\n\r");
			CLEAR_EVF(EVF_START_ADC);
		}

		if(EVF_IS_SET(EVF_ADC_CONV_FINISHED))
		{
			/* Get conversion result and start next ADC */
			vals[adc_chan] = ADC_get_conversion();
			adc_chan += 1;
			if(adc_chan < ADCs)
				ADC_start_conversion(adc_chan);
			CLEAR_EVF(EVF_ADC_CONV_FINISHED);
		}

		if(EVF_IS_SET(EVF_TIMER0_1MS_PASSED))
		{
			CLEAR_EVF(EVF_TIMER0_1MS_PASSED);
		}

		if(EVF_IS_SET(EVF_MAIN_LOOP_WAIT_FINISHED))
		{
			CLEAR_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
		}
	}
}

void main_loop2()
{
	timer0_init();
	timer0_start();
	ADC_init();
	#define ADCs 3
	uint8_t adc_chan = 0;
	ADC_val_t vals[ADCs];
	//ADC_val_t val;

	while(1)
	{
		/* Idle wait */
		while(event_flags == 0) {
			PORTD &= ~(1<<PD2);
		}
		PORTD |= (1<<PD2);
		us_start = timer0_us_since_start;

		if(EVF_IS_SET(EVF_START_ADC))
		{
			/* Start new conversion cycle */
			adc_chan = 0;
			ADC_start_conversion(adc_chan);

			/* Send out all gathered ADC values */
			UART_putd_32(us_start);
			UART_putc('\t');
			for(uint8_t i = 0; i<ADCs; i++)
			{
				UART_putd(vals[i]);
				UART_putc('\t');
			}
			UART_puts("\n\r");
			CLEAR_EVF(EVF_START_ADC);
		}

		if(EVF_IS_SET(EVF_ADC_CONV_FINISHED))
		{
			/* Get conversion result and start next ADC */
			vals[adc_chan] = ADC_get_conversion();
			adc_chan += 1;
			if(adc_chan < ADCs)
				ADC_start_conversion(adc_chan);
			CLEAR_EVF(EVF_ADC_CONV_FINISHED);
		}

		if(EVF_IS_SET(EVF_TIMER0_1MS_PASSED))
		{
			CLEAR_EVF(EVF_TIMER0_1MS_PASSED);
		}

		if(EVF_IS_SET(EVF_MAIN_LOOP_WAIT_FINISHED))
		{
			CLEAR_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
		}

		/* Measure task times */
		us_turnaround = timer0_us_since_start - us_start;
		us_delta = us_start - us_n1;
		us_n1 = us_start;
		//UART_putd_32(us_turnaround);
		//UART_putc('\t');
		//UART_putd_32(us_delta);
		//UART_puts("\n\r");
	}
}

/* Test function for the buffer.
   A local buffer is created and then continously written and read */
void buffer_test1()
{
	timer0_init();
	timer0_start();
	ADC_init();

#define BUF_LOCAL_SIZE	8

	BUFFER_DECLARE_AND_INIT(buf_local,int,BUF_LOCAL_SIZE);

	buf_local_type_t temp_write, temp_read;
	//buffer_result_t buf_result;
	temp_write = 0;
	while(1)
	{
		temp_write += 1;

		buffer_write(&buf_local,&temp_write);

		UART_puts("\n\rDATA: ");
		for(int i = 0; i < BUF_LOCAL_SIZE; i++)
		{
			UART_puts(" ");

			if(buf_local.index_w == i)
				UART_puts("w");
			if(buf_local.index_r == i)
				UART_puts("r");
			UART_putd(buf_local_data[i]);
		}

		if(buf_local.used > buf_local.index_max)
		{
			//UART_puts("\n\rDumping data: \n\r");
			while(buf_local.used > 0)
			{
				while(buffer_read(&buf_local,&temp_read)!=BUFFER_SUCCESS);

				_delay_ms(100);

				UART_puts("\n\rDATA: ");
				for(int i = 0; i < BUF_LOCAL_SIZE; i++)
				{
					UART_puts(" ");
					if(buf_local.index_w == i)
						UART_puts("w");
					if(buf_local.index_r == i)
						UART_puts("r");
					UART_putd(buf_local_data[i]);
				}
			}
		}
		PORTD ^= LED_ALIVE;
		_delay_ms(100);
	}
}

/* Test function for the buffer.
   The ADC interrupt will write to the buffer which is then read
	 in the main loop. */
void buffer_test2()
{
	timer0_init();
	timer0_start();
	ADC_init();

	BUFFER_DECLARE_AND_INIT(adct,ADC_t,8);
	ADC_t test;
	ADC_val_t x;

	//buffer_result_t buf_result;
	int temp;
	x = 0;

	while(1)
	{
		x += 1;

		if(EVF_IS_SET(EVF_START_ADC))
		{
			ADC_start_conversion(ADC_MUX_ADC0);
			CLEAR_EVF(EVF_START_ADC);
		}

		if(EVF_IS_SET(EVF_MAIN_LOOP_WAIT_FINISHED))
		{
			test.timestamp = timer0_us_since_start;
			test.val = ADC_single_shot(0);
			buffer_write(&adct,&test);
			CLEAR_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
		}

		if(adct.used > 4)
		{
			while(adct.used > 0)
			{
				buffer_read(&adct, &test);
				UART_puts("\n\r");
				UART_putd_32(test.timestamp);
				UART_puts(": ");
				UART_putd(test.val);
				UART_puts(" buffer used = ");
				UART_putd(adct.used);
			}
		}

		if(buf_adc.used > 4)
		{
			while(buf_adc.used > 0)
			{
				buffer_read(&buf_adc, &temp);
				UART_puts("\n\rADC = ");
				UART_putd(temp);
				UART_puts(" buffer used = ");
				UART_putd(buf_adc.used);
			}
		}
		PORTD ^= LED_ALIVE;
		//_delay_ms(500);
	}
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
			uart_buffer_write_long(&uart_buffer_tx, timer0_us_since_start);
			uart_buffer_write_string(&uart_buffer_tx, " ");
			while(uart_buffer_rx.used > 0)
			{
				c = uart_buffer_read(&uart_buffer_rx);
				uart_buffer_write(&uart_buffer_tx, c);
			}
			uart_kickout();
			while(uart_transmit_enabled());
			uart_buffer_write_long(&uart_buffer_tx, timer0_us_since_start);
			CLEAR_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
		}

		temp += 1;
		PORTB ^= (1<<PB5);
	}
}
