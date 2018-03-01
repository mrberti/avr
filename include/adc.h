#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

/*
 * ADMUX
 */
#define ADC_REF_SEL_MASK     (3<<REFS0)
#define ADC_REF_SEL_AREF     (0<<REFS0)
#define ADC_REF_SEL_AVCC     (1<<REFS0)
#define ADC_REF_SEL_INTERNAL (3<<REFS0)

#define ADC_ADJUST_MASK  (1<<ADLAR)
#define ADC_ADJUST_RIGHT (0<<ADLAR)
#define ADC_ADJUST_LEFT  (1<<ADLAR)

#define ADC_MUX_MASK (15<<MUX0)
#define ADC_MUX_ADC0 (0<<MUX0)
#define ADC_MUX_ADC1 (1<<MUX0)
#define ADC_MUX_ADC2 (2<<MUX0)
#define ADC_MUX_ADC3 (3<<MUX0)
#define ADC_MUX_ADC4 (4<<MUX0)
#define ADC_MUX_ADC5 (5<<MUX0)
#define ADC_MUX_ADC6 (6<<MUX0)
#define ADC_MUX_ADC7 (7<<MUX0)
#define ADC_MUX_TEMP (8<<MUX0)
#define ADC_MUX_1V1  (14<<MUX0)
#define ADC_MUX_0V   (15<<MUX0)

/*
 * ADCSRA
 */
#define ADC_ENABLE              (1<<ADEN)
#define ADC_START_CONVERSION    (1<<ADSC)
#define ADC_AUTO_TRIGGER_ENABLE (1<<ADATE)
#define ADC_INTERRUPT_FLAG      (1<<ADIF)
#define ADC_INTERRUPT_ENABLE    (1<<ADIE)

#define ADC_PRESCALER_MASK   (7<<ADPS0)
#define ADC_PRESCALER_CLK2   (0<<ADPS0)
#define ADC_PRESCALER_CLK4   (2<<ADPS0)
#define ADC_PRESCALER_CLK8   (3<<ADPS0)
#define ADC_PRESCALER_CLK16  (4<<ADPS0)
#define ADC_PRESCALER_CLK32  (5<<ADPS0)
#define ADC_PRESCALER_CLK64  (6<<ADPS0)
#define ADC_PRESCALER_CLK128 (7<<ADPS0)

/*
 * ADCL/H
 */

/*
 * ADCSRB
 */
#define ADC_ANALOG_COMP_MPLX_ENABLE  (1<<ACME)
#define ADC_ANALOG_COMP_MPLX_DISABLE (1<<ACME)

#define ADC_TRIGGER_SOURCE_MASK         (7<<ADTS0)
#define ADC_TRIGGER_SOURCE_FREE_RUNNING (0<<ADTS0)
#define ADC_TRIGGER_SOURCE_ANALOG_COMP  (1<<ADTS0)
#define ADC_TRIGGER_SOURCE_EXT_INT0     (2<<ADTS0)
#define ADC_TRIGGER_SOURCE_TC0_CMPA     (3<<ADTS0)
#define ADC_TRIGGER_SOURCE_TC0_OFVL     (4<<ADTS0)
#define ADC_TRIGGER_SOURCE_TC1_CMPB     (5<<ADTS0)
#define ADC_TRIGGER_SOURCE_TC1_OFVL     (6<<ADTS0)
#define ADC_TRIGGER_SOURCE_TC1_CAPTURE  (7<<ADTS0)

/*
 * DIDR0
 */
/* TODO: Digital Input Disable Register */

/*
 * GENERAL MACROS
 */
#define ADC_CONVERSION_IN_PROGRESS (ADCSRA&(1<<ADSC))

/*
 * GENERAL CONFIGURATIONS
 */
#define ADC_PRESCALER ADC_PRESCALER_CLK32

void ADC_init(void);
void ADC_set_running(void);
uint16_t ADC_single_shot(uint8_t adc_chan);

#endif //ADC_H
