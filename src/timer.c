#include "timer.h"
#include "events.h"

#include <avr/io.h>
#include <avr/interrupt.h>

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
  /* Interrupts should be set from outside... */
  //sei();
}

void timer0_stop()
{
  TCCR0B = TIMER0_CLK_DIV_STOP;
}

inline uint8_t timer0_get_cnt()
{
  return TCNT0;
}

inline void timer0_set_ocra(uint8_t ocr_val)
{
  OCR0A = ocr_val;
}

inline void timer0_set_ocrb(uint8_t ocr_val)
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
#ifndef DISABLE_INTERRUPT_COMPILE
ISR(TIMER0_OVF_vect)
{
}

ISR(TIMER0_COMPA_vect)
{
  /*
  TODO: Improvement ideas:
  - Use tick counter instead of re-calculating each timer step.
  The multiplication to get the actual us/ms values could be done in a slower
  task.
  - Use defines to enable or disable the corresponding timer flags
  */
  PORTD |= LED_TIMER0;
  timer0_us_since_start += TIMER0_US_PER_TICK;

  static uint16_t us_main_loop = 0;
  us_main_loop = us_main_loop+TIMER0_US_PER_TICK;
  if(us_main_loop >= MAIN_LOOP_TIME_US)
  {
    SET_EVF(EVF_MAIN_LOOP_WAIT_FINISHED);
    us_main_loop = 0;
  }

  static uint16_t us_1ms = 0;
  us_1ms = us_1ms+TIMER0_US_PER_TICK;
  if(us_1ms>=1000)
  {
    SET_EVF(EVF_TIMER0_1MS_PASSED);
    timer0_ms_since_start += 1;
    us_1ms = 0;
  }

  static uint16_t us_adc = 0;
  us_adc = us_adc+TIMER0_US_PER_TICK;
  if(us_adc>=ADC_LOOP_TIME_US)
  {
    SET_EVF(EVF_START_ADC);
    us_adc = 0;
  }

  PORTD &= ~LED_TIMER0;
}

ISR(TIMER0_COMPB_vect)
{
}
#endif //DISABLE_INTERRUPT_COMPILE
