#include "global.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "buffer.h"

#include <avr/io.h>
#include <util/delay.h>

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
#define ADCs 1
	ADC_val_t vals[ADCs];

	while(1)
	{
		/* Idle wait */
		while(timer0_adc_flag == 0) {
			PORTD |= (1<<PD2);
		}
		timer0_adc_flag = 0;

		UART_putd_32(timer0_us_since_start);
		UART_puts("\t");

		for(int i=0;i<ADCs;i++)
			vals[i] = ADC_single_shot(ADC_MUX_ADC1+i);

		for(int i=0;i<ADCs;i++)
		{
			UART_putd(vals[i]);
			UART_puts("\t");
		}

		UART_puts("\n\r");
		PORTD &= ~(1<<PD2);
	}
	return 0;
}
