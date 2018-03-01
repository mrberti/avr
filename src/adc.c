#include "adc.h"

#include <stdint.h>

void ADC_init()
{
  ADMUX =   ADC_REF_SEL_AVCC \
          | ADC_ADJUST_RIGHT;
  ADCSRA =  ADC_ENABLE \
          | ADC_PRESCALER;

  /* After enabling the ADC throw away the first few samples to let the ADC
     stabilize */
  for(uint8_t i = 1;i<10;i++)
    ADC_single_shot(ADC_MUX_ADC0);
}

void ADC_set_running()
{
  ADCSRA =  ADC_ENABLE \
          | ADC_START_CONVERSION \
          | ADC_AUTO_TRIGGER_ENABLE \
          | ADC_PRESCALER;

  ADCSRB =  ADC_ANALOG_COMP_MPLX_DISABLE \
          | ADC_TRIGGER_SOURCE_TC0_CMPA;
}

uint16_t ADC_single_shot(uint8_t adc_chan)
{
  ADMUX = (ADMUX & ~ADC_MUX_MASK) | adc_chan;
  ADCSRA |= ADC_START_CONVERSION;

  while(ADC_CONVERSION_IN_PROGRESS);

  return ADC;
}
