#ifndef _EVENTS_H
#define _EVENTS_H

#include <stdint.h>

#define EVF_START_ADC                      (1<<0)
#define EVF_ADC_CONV_FINISHED              (1<<1)
#define EVF_TIMER0_1MS_PASSED              (1<<2)
#define EVF_MAIN_LOOP_WAIT_FINISHED        (1<<3)
#define EVF_UART_BUFFER_TX_FINISHED        (1<<4)

#define EVF_IS_SET(flag) (event_flags  &  (flag))
#define CLEAR_EVF(flag)  (event_flags &= ~(flag))
#define SET_EVF(flag)    (event_flags |=  (flag))
#define RESET_EVF()      (event_flags = 0)

/* A global variable to signify that an event needs to be processed.
   Will be implemented in main.c */
extern volatile uint8_t event_flags;

#endif //_EVENTS_H
