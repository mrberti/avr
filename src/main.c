#include "global.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "buffer.h"

#include "template.h"

#include <avr/io.h>
#include <util/delay.h>

buffer_u8_t buf;

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

	buffer_u8_init(&buf);

	int i = 0;
	char c = 0;
	while(1)
	{
		i += 100;
		c += 10;
		UART_putd(test_template_int(i));
		UART_puts(", ");
		UART_putd(test_template_char(c));
		UART_puts("\n\r");
		// UART_putd_32(timer0_ovfl_interrupt_call_counter);
		// UART_puts("\t");
		// UART_putd_32(timer0_compa_interrupt_call_counter);
		// UART_puts("\t");
		// UART_putd_32(timer0_compb_interrupt_call_counter);
		// UART_puts("\t");
		// UART_putd_32(timer0_us_since_start);
		// UART_puts("\t");
		// ADC_t adc_val = ADC_single_shot_timestamp(ADC_MUX_ADC1);
		// UART_putd_32(adc_val.timestamp);
    // UART_puts("\t");
		// UART_putd(adc_val.val);
		// UART_puts("\t");
		// UART_putd(ADC_single_shot(ADC_MUX_ADC2));
		// i += 1;
		// uint8_t usage = 0;
		// UART_puts("i ");
		// UART_putd(i);
		// UART_puts(",");
		// UART_putd(buffer_u8_write(&buf, i));
		// UART_puts(",");
		// UART_putd(buffer_u8_usage(&buf, &usage));
		// UART_puts(",usage ");
		// UART_putd(usage);
		// if(usage == BUFFER_SIZE_MAX)
		// {
		// 	UART_puts(", values: ");
		// 	while(usage > 0)
		// 	{
		// 		uint8_t val = 0;
		// 		buffer_u8_read(&buf,&val);
		// 		buffer_u8_usage(&buf, &usage);
		// 		UART_putd(val);
		// 		UART_puts(",");
		// 		PORTD |= (1<<PD3);
		// 	}
		// }
		// PORTD &= ~(1<<PD3);
		// UART_puts("\n\r");
		timer0_wait_ms_blocking(100);
		//PORTD ^= (1<<PD2);
		//i = i+10;
		//timer0_set_ocrb(i);
	}
	return 0;
}
