#ifndef _GLOBAL_H
#define _GLOBAL_H

#ifndef F_CPU
#	define F_CPU 16000000UL
#endif //F_CPU

#include <stdint.h>

/* Set multiple bits while preserving other bits of the register */
#define SET_8BIT_REG(REG,MASK,VAL) (REG = (REG & ~(MASK)) | VAL)

/* A global variable to signify that an event needs to be processed.
   Will be implemented in main.c */
extern volatile uint8_t event_flags;

#define EVF_START_ADC                      (1<<0)
#define EVF_ADC_CONV_FINISHED              (1<<1)
#define EVF_TIMER0_1MS_PASSED              (1<<2)
#define EVF_TIMER0_MAIN_LOOP_WAIT_FINISHED (1<<3)

#define EVF_IS_SET(flag)   (event_flags  &  (flag))
#define CLEAR_EVF(flag) (event_flags &= ~(flag))
#define SET_EVF(flag)   (event_flags |=  (flag))
#define RESET_EVF()     (event_flags = 0)

#define TIMER0_TICK_RESOLUTION_US 100
#define MAIN_LOOP_TIME_US (1000)
#define ADC_LOOP_TIME_US  (100000)

#define LED_RED    (1<<PD2)
#define LED_YELLOW (1<<PD3)
#define LED_GREEN  (1<<PD4)

#define LED_ALIVE  (LED_RED)
#define LED_TIMER0 (LED_YELLOW)
#define LED_ADC    (LED_GREEN)

//#define DISABLE_INTERRUPT_COMPILE
#define UART_DEBUG_LEVEL_NONE    0
#define UART_DEBUG_LEVEL_ERROR   1
#define UART_DEBUG_LEVEL_WARNING 2
#define UART_DEBUG_LEVEL_NOTICE  3

#define UART_DEBUG_LEVEL UART_DEBUG_LEVEL_WARNING

#endif
