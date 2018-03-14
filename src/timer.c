#include "timer.h"
#include "events.h"
#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/**
 * GLOBAL VARIABLE INITIALIZATION
 */
volatile uint32_t timer0_us = 0;
volatile uint32_t timer0_ms = 0;

/**
 * FUNCTION IMPLEMENTATION
 */

void timer0_init()
{
  /* The output pin behaviour and PWM modes are defined here
     COM0x1:0 WGM01:0 */
  TCCR0A =   TIMER0_COM_NONPWM_NORMAL \
           | TIMER0_WGMA_CTC_OCRA;

  /* The CLK source and prescaler is selected here
     FOC0A:B WGM02 CS0[2:0] */
  TCCR0B =   TIMER0_WGMB_CTC_OCRA \
           | TIMER0_CLK_DIV_STOP;

  /* Set the interrupts
     OCIEB OCIEA TOIE */
  TIMSK0 =   TIMER0_CMP_A_INT_ENABLE \
           | TIMER0_CMP_B_INT_DISABLE \
           | TIMER0_OVFL_INT_DISABLE;

  timer0_set_ocra(TIMER0_OCRA_VAL);

  timer0_us = 0;
  timer0_ms = 0;
}

inline void timer0_start()
{
  SET_8BIT_REG(TCCR0B,TIMER0_CLK_DIV_MASK,TIMER0_CLK_DIV);
  /* Interrupts are required to be on so that the timer can run.
  Interrupts should be set from outside... */
  //sei();
}

inline void timer0_stop()
{
  TCCR0B = TIMER0_CLK_DIV_STOP;
}

void timer0_wait_ms_blocking(uint32_t wait_ms)
{
  uint32_t start_ms = 0;
  uint32_t end_ms = 0;
  uint32_t ovf_correct_ms = 0;
  uint32_t diff_ms = 0;

  start_ms = get_ms();
  end_ms = start_ms + wait_ms;

  /* In case of overflow, end_ms will be smaller than start_ms */
  if(end_ms < start_ms)
  {
    ovf_correct_ms = start_ms;
    end_ms = wait_ms;
  }

  /* blocking wait loop */
  do
  {
    diff_ms = get_ms() - ovf_correct_ms;
  } while(diff_ms < end_ms);
}

// ISR(TIMER0_OVF_vect)
// {
// }

ISR(TIMER0_COMPA_vect)
{
  timer0_us += TIMER0_US_PER_TICK;

  static uint16_t us_1ms = 0;
  us_1ms = us_1ms+TIMER0_US_PER_TICK;
  if(us_1ms>=1000)
  {
    SET_EVF(EVF_TIMER0_1MS_PASSED);
    timer0_ms += 1;
    us_1ms = 0;
  }

  static uint16_t us_main_loop = 0;
  us_main_loop = us_main_loop+TIMER0_US_PER_TICK;
  if(us_main_loop >= MAIN_LOOP_TIME_US)
  {
    SET_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
    us_main_loop = 0;
  }

  static uint16_t us_adc = 0;
  us_adc = us_adc+TIMER0_US_PER_TICK;
  if(us_adc>=ADC_LOOP_TIME_US)
  {
    SET_EVF(EVF_START_ADC);
    us_adc = 0;
  }
}

// ISR(TIMER0_COMPB_vect)
// {
// }

void timer0_test()
{
  timer0_init();
  timer0_start();
  sei();
  while(1)
  {
    if(EVF_IS_SET(EVF_MAIN_LOOP_WAIT_FINISHED))
    {
      UART_puts("\n\r");
      UART_putd_32(get_us());
      UART_puts("\t");
      UART_putd_32(get_ms());
      CLEAR_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
    }
  }
}
