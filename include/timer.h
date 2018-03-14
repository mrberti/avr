#ifndef _TIMER_H
#define _TIMER_H

#include "global.h"

#include <stdint.h>

/*
 * TCCR0A
 */
/* WGM0[2:0] set to normal CTC mode (non PWM) */
#define TIMER0_COM_NONPWM_NORMAL       (0<<COM0A0)
#define TIMER0_COM_NONPWM_OC0A_TOGGLE  (1<<COM0A0)
#define TIMER0_COM_NONPWM_OC0A_CLEAR   (2<<COM0A0)
#define TIMER0_COM_NONPWM_OC0A_SET     (3<<COM0A0)

/* WGM0[1:0] set to Fast PWM */
/* TODO */

/* WGM0[2:0] set to phase correct PWM */
/* TODO */

/* WGM0[1:0] set to Fast PWM */
/* TODO */

/* WGMA for TCCR0A, WGMB for TCCR0B*/
#define TIMER0_WGMA_NORMAL                  (0<<WGM00)
#define TIMER0_WGMB_NORMAL                  (0<<WGM02)
#define TIMER0_WGMA_PWM_PHASE_CORRECT       (1<<WGM00)
#define TIMER0_WGMB_PWM_PHASE_CORRECT       (0<<WGM02)
#define TIMER0_WGMA_CTC_OCRA                (2<<WGM00)
#define TIMER0_WGMB_CTC_OCRA                (0<<WGM02)
#define TIMER0_WGMA_PWM_FAST                (3<<WGM00)
#define TIMER0_WGMB_PWM_FAST                (0<<WGM02)
#define TIMER0_WGMA_PWM_PHASE_CORRECT_OCRA  (1<<WGM00)
#define TIMER0_WGMB_PWM_PHASE_CORRECT_OCRA  (1<<WGM02)
#define TIMER0_WGMA_PWM_FAST_OCRA           (3<<WGM00)
#define TIMER0_WGMB_PWM_FAST_OCRA           (1<<WGM02)

/*
 * TCCR0B
 */
#define TIMER0_CLK_DIV_MASK           (7<<CS00)
#define TIMER0_CLK_DIV_STOP           (0<<CS00)
#define TIMER0_CLK_DIV_1              (1<<CS00)
#define TIMER0_CLK_DIV_8              (2<<CS00)
#define TIMER0_CLK_DIV_64             (3<<CS00)
#define TIMER0_CLK_DIV_256            (4<<CS00)
#define TIMER0_CLK_DIV_1024           (5<<CS00)
#define TIMER0_CLK_DIV_EXT_T0_FALLING (6<<CS00)
#define TIMER0_CLK_DIV_EXT_T0_RISING  (7<<CS00)

/* FOC0 (Force Output Compare) not implemented */

/*
 * TIMSK0
 */
#define TIMER0_OVFL_INT_DISABLE   (0<<TOIE0)
#define TIMER0_OVFL_INT_ENABLE    (1<<TOIE0)
#define TIMER0_CMP_A_INT_DISABLE  (0<<OCIE0A)
#define TIMER0_CMP_A_INT_ENABLE   (1<<OCIE0A)
#define TIMER0_CMP_B_INT_DISABLE  (0<<OCIE0B)
#define TIMER0_CMP_B_INT_ENABLE   (1<<OCIE0B)

/*
 * GTCCR
 */
/* TODO */

/*
 * TIFR
 */
/* TODO */

/**
 * TIMER CONFIGURATION
 */
/* 10 us resolution timer: OCRA=19,  CLKDIV=8*/
#if F_CPU == 16000000UL
#if TIMER0_TICK_RESOLUTION_US == 10
# define TIMER0_OCRA_VAL    (19)
# define TIMER0_CLK_DIV_VAL (8)
# define TIMER0_CLK_DIV     (TIMER0_CLK_DIV_8)
/* 20 us resolution timer: OCRA=39,  CLKDIV=8*/
#elif TIMER0_TICK_RESOLUTION_US == 20
# define TIMER0_OCRA_VAL    (39)
# define TIMER0_CLK_DIV_VAL (8)
# define TIMER0_CLK_DIV     (TIMER0_CLK_DIV_8)
/* 50 us resolution timer: OCRA=99,  CLKDIV=8*/
#elif TIMER0_TICK_RESOLUTION_US == 50
# define TIMER0_OCRA_VAL    (99)
# define TIMER0_CLK_DIV_VAL (8)
# define TIMER0_CLK_DIV     (TIMER0_CLK_DIV_8)
/* 100 us resolution timer: OCRA=199,  CLKDIV=8*/
#elif TIMER0_TICK_RESOLUTION_US == 100
# define TIMER0_OCRA_VAL    (199)
# define TIMER0_CLK_DIV_VAL (8)
# define TIMER0_CLK_DIV     (TIMER0_CLK_DIV_8)
/* 200 us resolution timer: OCRA=49,  CLKDIV=64*/
#elif TIMER0_TICK_RESOLUTION_US == 200
# define TIMER0_OCRA_VAL    (49)
# define TIMER0_CLK_DIV_VAL (64)
# define TIMER0_CLK_DIV     (TIMER0_CLK_DIV_64)
/* 1 ms resolution timer: OCRA=249,  CLKDIV=64*/
#elif TIMER0_TICK_RESOLUTION_US == 1000
# define TIMER0_OCRA_VAL (249)
# define TIMER0_CLK_DIV_VAL (64)
# define TIMER0_CLK_DIV (TIMER0_CLK_DIV_64)
#else
# error "No correct TIMER0_TICK_RESOLUTION_US is defined."
#endif //TIMER0TIMER0_TICK_RESOLUTION_US
#else
# error "F_CPU not supported"
#endif //F_CPU

#define TIMER0_US_PER_TICK (TIMER0_CLK_DIV_VAL*(TIMER0_OCRA_VAL+1UL)/(F_CPU/1000000UL))

/*
 * GLOBAL VARIABLES
 */
extern volatile uint32_t timer0_us;
extern volatile uint32_t timer0_ms;

/*
 * FUNCTION DECLARATION
 */
void timer0_init(void);
void timer0_start(void);
void timer0_stop(void);

void timer0_set_ocra(uint8_t ocr_val);
void timer0_set_ocrb(uint8_t ocr_val);
void timer0_wait_ms_blocking(uint32_t wait_ms);

#define timer0_get_cnt() (TCNT0)
#define timer0_set_ocra(ocr_val) (OCR0A = ocr_val)
#define timer0_set_ocrb(ocr_val) (OCR0B = ocr_val)

#define get_us() (timer0_us)
#define get_ms() (timer0_ms)

void timer0_test(void);

#endif //TIMER_H
