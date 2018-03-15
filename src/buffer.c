#include "buffer.h"
#include "global.h"

/* Includes for the main test */
#include "timer.h"
#include "adc.h"
#include "uart.h"
#include "events.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#if UART_DEBUG_LEVEL > UART_DEBUG_LEVEL_NONE
# include "uart.h"
# include "timer.h"
#endif

inline write_type(char)
{
  *(char*)(data+index*sizeof(char)) = *(char*)val;
}

inline write_type(int)
{
  *(int*)(data+index*sizeof(int)) = *(int*)val;
}

inline write_type(long)
{
  *(long*)(data+index*sizeof(long)) = *(long*)val;
}

inline read_type(char)
{
  *(char*)val = *(char*)(data+index*sizeof(char));
}

inline read_type(int)
{
  *(int*)val = *(int*)(data+index*sizeof(int));
}

inline read_type(long)
{
  *(long*)val = *(long*)(data+index*sizeof(long));
}

buffer_result_t buffer_write(buffer_t *buf, void *val)
{
  /* At first it must be checked if the buffer is locked */
  if(buf->flags & BUFFER_FLAG_LOCK)
  {
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_WARNING
    UART_puts("\n\r");
    UART_putd_32(get_us());
    UART_puts(" W: BUFFER_LOCKED");
#endif
    return BUFFER_LOCKED;
  }
  buf->flags |= BUFFER_FLAG_LOCK;

  /* Check if there is enough space in the buffer. Otherwise release the lock and
  return */
  buffer_index_t index_max = buf->index_max;
  buffer_index_t used = buf->used;
  if(used > index_max)
  {
    buf->flags &= ~BUFFER_FLAG_LOCK;
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_NOTICE
    UART_puts("\n\r");
    UART_putd_32(get_us());
    UART_puts(" W: BUFFER_FULL");
#endif
    return BUFFER_FULL;
  }

  /* Increase the write index, check for overflow and write to the buffer */
  buffer_index_t index_w = buf->index_w;
  if(index_w < index_max)
    index_w += 1;
  else
    index_w = 0;
  buf->write_func(buf->data, index_w, val);
  used += 1;

  /* Finally, write the temporary values back to the buffer, unlock the buffer
  and return success */
  buf->index_w = index_w;
  buf->used    = used;
  buf->flags  &= ~BUFFER_FLAG_LOCK;
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_NOTICE
    UART_puts("\n\r");
    UART_putd_32(get_us());
    UART_puts(" W: OK. used: ");
    UART_putd(used);
    UART_puts("/");
    UART_putd(index_max+1);
    UART_puts(" index_w: ");
    UART_putd(index_w);
#endif
  return BUFFER_SUCCESS;
}

buffer_result_t buffer_read(buffer_t *buf, void *val)
{
  /* At first it must be checked if the buffer is locked */
  if(buf->flags & BUFFER_FLAG_LOCK)
  {
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_WARNING
    UART_puts("\n\r");
    UART_putd_32(get_us());
    UART_puts(" R: BUFFER_LOCKED");
#endif
    return BUFFER_LOCKED;
  }
  buf->flags |= BUFFER_FLAG_LOCK;

  /* Check if there is data in the buffer. Otherwise release the lock and
  return */
  buffer_index_t used = buf->used;
  if(used == 0)
  {
    buf->flags &= ~BUFFER_FLAG_LOCK;
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_NOTICE
    UART_puts("\n\r");
    UART_putd_32(get_us());
    UART_puts(" R: BUFFER_EMPTY");
#endif
    return BUFFER_EMPTY;
  }

  /* Increase the read index, check for overflow and read from the buffer */
  buffer_index_t index_r = buf->index_r;
  buffer_index_t index_max = buf->index_max;
  if(index_r <  index_max)
    index_r += 1;
  else
    index_r = 0;
  buf->read_func(buf->data, index_r, val);
  used -= 1;

  /* Finally, write the temporary values back to the bufferunlock the buffer
  and return success */
  buf->index_r = index_r;
  buf->used    = used;
  buf->flags  &= ~BUFFER_FLAG_LOCK;
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_NOTICE
    UART_puts("\n\r");
    UART_putd_32(get_us());
    UART_puts(" R: OK. used: ");
    UART_putd(used);
    UART_puts("/");
    UART_putd(index_max+1);
    UART_puts(" index_r: ");
    UART_putd(index_r);
#endif
  return BUFFER_SUCCESS;
}



/* Test function for the buffer.
   A local buffer is created and then continously written and read */
void buffer_test1()
{
	timer0_init();
	timer0_start();
	ADC_init();
	sei();

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
	sei();

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
			test.timestamp = get_us();
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
