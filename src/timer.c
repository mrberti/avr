#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>



#include "uart.h"

/**
 * GLOBAL VARIABLE INITIALIZATION
 */
volatile uint32_t timer0_us_since_start = 0;
volatile uint32_t timer0_ms_since_start = 0;

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
           | TIMER0_CMP_B_INT_ENABLE \
           | TIMER0_OVFL_INT_ENABLE;

  timer0_set_ocra(TIMER0_OCRA_VAL);

  timer0_us_since_start = 0;
  timer0_ms_since_start = 0;
}

void timer0_start()
{
  /* TODO: Save previous state of TCCR0B*/
  TCCR0B = TIMER0_CLK_DIV;
  /* Interrupts are required to be on so that the timer can run */
  sei();
}

void timer0_stop()
{
  TCCR0B = TIMER0_CLK_DIV_STOP;
}

uint8_t timer0_get_cnt()
{
  return TCNT0;
}

void timer0_set_ocra(uint8_t ocr_val)
{
  OCR0A = ocr_val;
}

void timer0_set_ocrb(uint8_t ocr_val)
{
  OCR0B = ocr_val;
}

void timer0_wait_ms_blocking(uint32_t wait_ms)
{
  uint32_t start_ms = 0;
  uint32_t end_ms = 0;
  uint32_t ovf_correct_ms = 0;
  uint32_t diff_ms = 0;

  start_ms = timer0_ms_since_start;
  end_ms = start_ms + wait_ms;

  /* In case of overflow, end_ms will be smaller than start_ms */
  if(start_ms < end_ms)
  {
    ovf_correct_ms = 0;
  }
  else
  {
    ovf_correct_ms = start_ms;
    end_ms = wait_ms;
  }

  /* blocking wait loop */
  do
  {
    diff_ms = timer0_ms_since_start - ovf_correct_ms;
  } while(diff_ms < end_ms);
}

/**
 * INTERRUPT ROUTINES
 */
ISR(TIMER0_OVF_vect)
{
}

ISR(TIMER0_COMPA_vect)
{
  timer0_us_since_start += TIMER0_US_PER_TICK;

  /* set mili seconds */
  static uint16_t us = 0;
  us = us+TIMER0_US_PER_TICK;
  if(us>=1000)
  {
    timer0_ms_since_start += 1;
    us = 0;
  }
}

ISR(TIMER0_COMPB_vect)
{
}
