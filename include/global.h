#ifndef _GLOBAL_H
#define _GLOBAL_H

#ifndef F_CPU
#	define F_CPU 16000000UL
#endif //F_CPU

//#include <stdint.h>

/* Set multiple bits while preserving other bits of the register */
#define SET_8BIT_REG(REG,MASK,VAL) (REG = (REG & ~(MASK)) | VAL)

#define TIMER0_TICK_RESOLUTION_US (200)
#define MAIN_LOOP_TIME_US         (10000)
#define ADC_LOOP_TIME_US          (10000)

#define LED_RED    (1<<PD2)
#define LED_YELLOW (1<<PD3)
#define LED_GREEN  (1<<PD4)
#define LED_BOARD  (1<<PB0)

#define LED_ALIVE   (LED_RED)
#define LED_TIMER0  (LED_YELLOW)
#define LED_ADC     (LED_GREEN)
#define LED_UART_RX (LED_RED)
#define LED_UART_TX (LED_GREEN)

#define UART_DEBUG_LEVEL_NONE    0
#define UART_DEBUG_LEVEL_ERROR   1
#define UART_DEBUG_LEVEL_WARNING 2
#define UART_DEBUG_LEVEL_NOTICE  3

#define UART_DEBUG_LEVEL UART_DEBUG_LEVEL_NONE
//#define DISABLE_INTERRUPT_COMPILE

#endif
