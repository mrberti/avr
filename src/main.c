#include "global.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "buffer.h"
#include "events.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t event_flags;

uint32_t us_n1 = 0;
uint32_t us_turnaround = 0;
uint32_t us_delta = 0;
uint32_t us_start = 0;

// static void main_loop2(void);
// static void main_loop1(void);
//
// void main_loop1()
// {
// 	timer0_init();
// 	timer0_start();
// 	ADC_init();
//
// 	sei();
//
// 	#define ADCs 3
// 	uint8_t adc_chan = 0;
// 	ADC_val_t vals[ADCs];
// 	//ADC_val_t val;
//
// 	while(1)
// 	{
// 		/* Idle wait */
// 		while(event_flags == 0) {
// 			PORTD &= ~(1<<PD2);
// 		}
// 		PORTD |= (1<<PD2);
//
// 		if(EVF_IS_SET(EVF_START_ADC))
// 		{
// 			/* Start new conversion cycle */
// 			adc_chan = 0;
// 			ADC_start_conversion(adc_chan);
//
// 			/* Send out all gathered ADC values */
// 			UART_putd_32(get_us());
// 			UART_putc('\t');
// 			for(uint8_t i = 0; i<ADCs; i++)
// 			{
// 				UART_putd(vals[i]);
// 				UART_putc('\t');
// 			}
// 			UART_puts("\n\r");
// 			CLEAR_EVF(EVF_START_ADC);
// 		}
//
// 		if(EVF_IS_SET(EVF_ADC_CONV_FINISHED))
// 		{
// 			/* Get conversion result and start next ADC */
// 			vals[adc_chan] = ADC_get_conversion();
// 			adc_chan += 1;
// 			if(adc_chan < ADCs)
// 				ADC_start_conversion(adc_chan);
// 			CLEAR_EVF(EVF_ADC_CONV_FINISHED);
// 		}
//
// 		if(EVF_IS_SET(EVF_TIMER0_1MS_PASSED))
// 		{
// 			CLEAR_EVF(EVF_TIMER0_1MS_PASSED);
// 		}
//
// 		if(EVF_IS_SET(EVF_MAIN_LOOP_WAIT_FINISHED))
// 		{
// 			CLEAR_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
// 		}
// 	}
// }
//
// void main_loop2()
// {
// 	timer0_init();
// 	timer0_start();
// 	ADC_init();
// 	sei();
// 	#define ADCs 3
// 	uint8_t adc_chan = 0;
// 	ADC_val_t vals[ADCs];
// 	//ADC_val_t val;
//
// 	while(1)
// 	{
// 		/* Idle wait */
// 		while(event_flags == 0) {
// 			PORTD &= ~(1<<PD2);
// 		}
// 		PORTD |= (1<<PD2);
// 		us_start = get_us();
//
// 		if(EVF_IS_SET(EVF_START_ADC))
// 		{
// 			/* Start new conversion cycle */
// 			adc_chan = 0;
// 			ADC_start_conversion(adc_chan);
//
// 			/* Send out all gathered ADC values */
// 			UART_putd_32(us_start);
// 			UART_putc('\t');
// 			for(uint8_t i = 0; i<ADCs; i++)
// 			{
// 				UART_putd(vals[i]);
// 				UART_putc('\t');
// 			}
// 			UART_puts("\n\r");
// 			CLEAR_EVF(EVF_START_ADC);
// 		}
//
// 		if(EVF_IS_SET(EVF_ADC_CONV_FINISHED))
// 		{
// 			/* Get conversion result and start next ADC */
// 			vals[adc_chan] = ADC_get_conversion();
// 			adc_chan += 1;
// 			if(adc_chan < ADCs)
// 				ADC_start_conversion(adc_chan);
// 			CLEAR_EVF(EVF_ADC_CONV_FINISHED);
// 		}
//
// 		if(EVF_IS_SET(EVF_TIMER0_1MS_PASSED))
// 		{
// 			CLEAR_EVF(EVF_TIMER0_1MS_PASSED);
// 		}
//
// 		if(EVF_IS_SET(EVF_MAIN_LOOP_WAIT_FINISHED))
// 		{
// 			CLEAR_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
// 		}
//
// 		/* Measure task times */
// 		us_turnaround = get_us() - us_start;
// 		us_delta = us_start - us_n1;
// 		us_n1 = us_start;
// 		//UART_putd_32(us_turnaround);
// 		//UART_putc('\t');
// 		//UART_putd_32(us_delta);
// 		//UART_puts("\n\r");
// 	}
// }

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

	timer0_test();
	//uart_buffered_test();
	//buffer_test2();
	//buffer_test1();
	//main_loop1();
	//main_loop2();

	/* Never reached */
	return 0;
}
