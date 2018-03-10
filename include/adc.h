#ifndef _ADC_H
#define _ADC_H

#include "global.h"
#include "buffer.h"

#include <avr/io.h>

/*
 * CONFIGURATION
 */
#define ADC_USE_INTERRUPTS (1)
//#define ADC_USE_8BIT (1)

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
#define ADC_INTERRUPT_DISABLE   (0<<ADIE)

#define ADC_PRESCALER_MASK   (7<<ADPS0)
#define ADC_PRESCALER_CLK2   (0<<ADPS0)
#define ADC_PRESCALER_CLK4   (2<<ADPS0)
#define ADC_PRESCALER_CLK8   (3<<ADPS0)
#define ADC_PRESCALER_CLK16  (4<<ADPS0)
#define ADC_PRESCALER_CLK32  (5<<ADPS0)
#define ADC_PRESCALER_CLK64  (6<<ADPS0)
#define ADC_PRESCALER_CLK128 (7<<ADPS0)

/*
 * ADCSRB
 */
#define ADC_ANALOG_COMP_MPLX_ENABLE  (1<<ACME)
#define ADC_ANALOG_COMP_MPLX_DISABLE (0<<ACME)

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
#ifdef ADC_USE_8BIT
# define ADC_CONVERSION_REG (ADCH)
#else
# define ADC_CONVERSION_REG (ADC)
#endif

/*
 * GENERAL CONFIGURATIONS
 */
#ifdef ADC_USE_8BIT
# define ADC_ADJUST ADC_ADJUST_LEFT
#else
# define ADC_ADJUST ADC_ADJUST_RIGHT
#endif //ADC_USE_8BIT

#define ADC_REF_SEL ADC_REF_SEL_AVCC
#define ADC_PRESCALER ADC_PRESCALER_CLK32

#ifdef ADC_USE_INTERRUPTS
# define ADC_INTERRUPT ADC_INTERRUPT_ENABLE
#else
# define ADC_INTERRUPT ADC_INTERRUPT_DISABLE
#endif //ADC_USE_INTERRUPTS

/*
 * TYPEDEFS
 */
#ifdef ADC_USE_8BIT
typedef uint8_t ADC_val_t;
#else
typedef uint16_t ADC_val_t;
#endif //ADC_USE_8BIT

typedef struct ADC_t_struct {
  uint32_t timestamp;
  ADC_val_t val;
} ADC_t;

/**
 * GLOBAL VARIBALES
 */
/*
#ifdef ADC_USE_8BIT
typedef buffer_u8_t adc_buf_t;
#else
typedef buffer_u16_t adc_buf_t;
#endif
extern adc_buf_t adc_buffer;
*/
#define ADC_BUFFERSIZE 8
extern buffer_t buf_adc;
extern int buf_adc_data[ADC_BUFFERSIZE];

/*
 * FUNCTION PROTOTYPES
 */
void ADC_init(void);
void ADC_set_running(void);
void ADC_start_conversion(uint8_t adc_chan);
ADC_val_t ADC_get_conversion(void);
ADC_val_t ADC_single_shot(uint8_t adc_chan);
ADC_t ADC_single_shot_timestamp(uint8_t adc_chan);

#endif //ADC_H
