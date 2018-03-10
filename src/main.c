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

int main(void)
{
	DDRB = (1<<PB5);
	DDRD = (1<<PD4) | (1<<PD3) | (1<<PD2);

	UART_init(UART_UBRR_500k);
	UART_enable();
	UART_puts("\033[2J\n\r\n\rUART_initialized.\n\r\n\r");

	//timer0_init();
	//timer0_start();

	ADC_init();

	sei();

	buffer_test1();
	buffer_test2();
	main_loop1();
	main_loop2();

	/* Never reached */
	return 0;
}

void main_loop1()
{
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

		if(EVF_SET(EVF_START_ADC))
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

		if(EVF_SET(EVF_ADC_CONV_FINISHED))
		{
			/* Get conversion result and start next ADC */
			vals[adc_chan] = ADC_get_conversion();
			adc_chan += 1;
			if(adc_chan < ADCs)
				ADC_start_conversion(adc_chan);
			CLEAR_EVF(EVF_ADC_CONV_FINISHED);
		}

		if(EVF_SET(EVF_TIMER0_1MS_PASSED))
		{
			CLEAR_EVF(EVF_TIMER0_1MS_PASSED);
		}

		if(EVF_SET(EVF_TIMER0_MAIN_LOOP_WAIT_FINISHED))
		{
			CLEAR_EVF(EVF_TIMER0_MAIN_LOOP_WAIT_FINISHED);
		}
	}
}

void main_loop2()
{
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

		if(EVF_SET(EVF_START_ADC))
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

		if(EVF_SET(EVF_ADC_CONV_FINISHED))
		{
			/* Get conversion result and start next ADC */
			vals[adc_chan] = ADC_get_conversion();
			adc_chan += 1;
			if(adc_chan < ADCs)
				ADC_start_conversion(adc_chan);
			CLEAR_EVF(EVF_ADC_CONV_FINISHED);
		}

		if(EVF_SET(EVF_TIMER0_1MS_PASSED))
		{
			CLEAR_EVF(EVF_TIMER0_1MS_PASSED);
		}

		if(EVF_SET(EVF_TIMER0_MAIN_LOOP_WAIT_FINISHED))
		{
			CLEAR_EVF(EVF_TIMER0_MAIN_LOOP_WAIT_FINISHED);
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
#define BUF_LOCAL_SIZE	8

	BUFFER_DECLARE_AND_INIT(buf_local,int,BUF_LOCAL_SIZE);

	buf_local_type_t temp_write, temp_read;
	while(1)
	{
		temp_write += 10;
		buffer_write(&buf_local,&temp_write);

		if(buf_local.index_w == 0)
		{
			UART_puts("\n\r");

			for(int x = 0; x<BUF_LOCAL_SIZE;x++)
			{
				buffer_read(&buf_local, &temp_read);
				//UART_putd_32(buf_local_data[x]);
				UART_putd_32(temp_read);
				UART_puts("\t");
			}
		}
		//UART_puts("\n\r");
		PORTD ^= (1<<LED_ALIVE);
		_delay_ms(100);
	}
}

/* Test function for the buffer.
   The ADC interrupt will write to the buffer which is then read
	 in the main loop. */
void buffer_test2()
{
	while(1)
	{
		ADC_start_conversion(ADC_MUX_ADC0);

		if(buf_adc.index_w == 0)
		{
			UART_puts("\n\r");

			for(int x = 0; x<ADC_BUFFERSIZE;x++)
			{
				int temp;
				buffer_read(&buf_adc, &temp);
				//UART_putd_32(buf_adc_data[x]);
				UART_putd_32(temp);
				UART_puts("\t");
			}
		}
		PORTD ^= (1<<LED_ALIVE);
		_delay_ms(100);
	}
}
