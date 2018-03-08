#include "global.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "buffer.h"

#include <avr/io.h>
#include <util/delay.h>

volatile uint8_t event_flags;

uint32_t us_n1 = 0;
uint32_t us_turnaround = 0;
uint32_t us_delta = 0;
uint32_t us_start = 0;

static void main_loop1(void);
static void main_loop2(void);

int main(void)
{
	DDRB = (1<<PB5);
	DDRD = (1<<PD3) | (1<<PD2);

	UART_init(UART_UBRR_500k);
	UART_enable();
	UART_puts("\n\r\n\rUART_initialized.\n\r\n\r");

	timer0_init();
	timer0_start();

	ADC_init();

	main_loop1();
	//main_loop2();

	/* Never reached */
	return 0;
}

void main_loop1()
{
	#define ADCs 3
	uint8_t adc_chan = 0;
	ADC_val_t vals[ADCs];
	ADC_val_t val;

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
	ADC_val_t val;

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
