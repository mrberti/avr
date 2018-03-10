#include "adc.h"
#include "global.h"
#include "timer.h"
#include "buffer.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//adc_buf_t adc_buffer;
BUFFER_DECLARE(buf_adc,int,ADC_BUFFERSIZE);

void ADC_init()
{
  ADMUX =   ADC_REF_SEL \
          | ADC_ADJUST;
  ADCSRA =  ADC_ENABLE \
          | ADC_PRESCALER \
          | ADC_INTERRUPT;

  /* After enabling the ADC throw away the first few samples to let the ADC
     stabilize */
  /* TODO: This step seems to be victim to optimization */
  for(uint8_t i = 1;i<10;i++)
    ADC_single_shot(ADC_MUX_0V);

  /* Initialize the ADC buffer */
  BUFFER_INIT(buf_adc,int,ADC_BUFFERSIZE);
}

void ADC_set_running()
{
  /* TODO: Test this function */
  ADCSRA =  ADC_ENABLE \
          | ADC_START_CONVERSION \
          | ADC_AUTO_TRIGGER_ENABLE \
          | ADC_PRESCALER;

  ADCSRB =  ADC_ANALOG_COMP_MPLX_DISABLE \
          | ADC_TRIGGER_SOURCE_TC0_CMPA;
}

ADC_val_t ADC_single_shot(uint8_t adc_chan)
{
  SET_8BIT_REG(ADMUX,ADC_MUX_MASK,adc_chan);
  ADCSRA |= ADC_START_CONVERSION;

  /* This function blocks while the conversion is not finished */
  while(ADC_CONVERSION_IN_PROGRESS);

  return ADC_CONVERSION_REG;
}

void ADC_start_conversion(uint8_t adc_chan)
{
  /* As the results will be read from the interrupt, they must be enabled before
  calling this function */
  PORTD |= (1<<LED_ADC);
  SET_8BIT_REG(ADMUX,ADC_MUX_MASK,adc_chan);
  ADCSRA |= ADC_START_CONVERSION;
}

ADC_val_t ADC_get_conversion(void)
{
  return ADC_CONVERSION_REG;
}

ADC_t ADC_single_shot_timestamp(uint8_t adc_chan)
{
  ADC_t adc_val;
  adc_val.val = ADC_single_shot(adc_chan);
  adc_val.timestamp = timer0_us_since_start;
  return adc_val;
}

#ifdef ADC_USE_INTERRUPTS
#ifndef DISABLE_INTERRUPT_COMPILE
ISR(ADC_vect)
{
  SET_EVF(EVF_ADC_CONV_FINISHED);
  //ADC_val_t adc_val = ADC_CONVERSION_REG;
  int adc_val = ADC_CONVERSION_REG;
  buffer_write(&buf_adc, &adc_val);
  PORTD &= ~(1<<LED_ADC);
}
#endif //DISABLE_INTERRUPT_COMPILE
#endif //ADC_USE_INTERRUPTS
