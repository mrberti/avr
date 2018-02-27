#ifndef UART_H_
#define UART_H_

/**
 * \brief Simple functions to send data via the UART
 * 		  Tested on AVR atmega328p32 (arduino)
 * \author	Simon Bertling
 */
#include "global.h"
#include <inttypes.h>

/**
 * Macros for calculating the UBBR values.
 * Use clever rounding
 */
#define UART_UBRR_CALC(baud)       ((F_CPU+baud*8)/(baud*16)-1)
#define UART_BAUD_REAL_CALC(ubrr)  (F_CPU/(16*(ubrr+1)))
#define BAUD_ERROR_CALC(x)    ((BAUD_REAL_CALC(x)*1000)/UART_UBRR_CALC(x))

/* Standard baud rates */
#define UART_UBRR_9k6   (UART_UBRR_CALC(9600UL))
#define UART_UBRR_14k4  (UART_UBRR_CALC(14400UL))
#define UART_UBRR_19k2  (UART_UBRR_CALC(19200UL))
#define UART_UBRR_28k8  (UART_UBRR_CALC(28800UL))
#define UART_UBRR_38k4  (UART_UBRR_CALC(38400UL))
#define UART_UBRR_57k6  (UART_UBRR_CALC(57600UL))
#define UART_UBRR_76k8  (UART_UBRR_CALC(76800UL))
#define UART_UBRR_115k2 (UART_UBRR_CALC(115200UL))
#define UART_UBRR_230k4 (UART_UBRR_CALC(230400UL))
#define UART_UBRR_250k  (UART_UBRR_CALC(250000UL))
#define UART_UBRR_500k  (UART_UBRR_CALC(500000UL))
#define UART_UBRR_1000k (UART_UBRR_CALC(1000000UL))

/* Effective baud rate due to rounding issues */
#define UART_BAUD_REAL_9k6   (UART_BAUD_REAL_CALC(UART_UBRR_9k6))
#define UART_BAUD_REAL_14k4  (UART_BAUD_REAL_CALC(UART_UBRR_14k4))
#define UART_BAUD_REAL_19k2  (UART_BAUD_REAL_CALC(UART_UBRR_19k2))
#define UART_BAUD_REAL_28k8  (UART_BAUD_REAL_CALC(UART_UBRR_28k8))
#define UART_BAUD_REAL_38k4  (UART_BAUD_REAL_CALC(UART_UBRR_38k4))
#define UART_BAUD_REAL_57k6  (UART_BAUD_REAL_CALC(UART_UBRR_57k6))
#define UART_BAUD_REAL_76k8  (UART_BAUD_REAL_CALC(UART_UBRR_76k8))
#define UART_BAUD_REAL_115k2 (UART_BAUD_REAL_CALC(UART_UBRR_115k2))
#define UART_BAUD_REAL_230k4 (UART_BAUD_REAL_CALC(UART_UBRR_230k4))
#define UART_BAUD_REAL_250k  (UART_BAUD_REAL_CALC(UART_UBRR_250k))
#define UART_BAUD_REAL_500k  (UART_BAUD_REAL_CALC(UART_UBRR_500k))
#define UART_BAUD_REAL_1000k (UART_BAUD_REAL_CALC(UART_UBRR_1000k))

/**
 * \brief Initialize the UART
 * \param ubrr: The 16 bit value for the UBRR register
 * \returns No return value
 */
void UART_init(uint16_t ubrr);

/**
 * \brief Deactivate the UART
 * \param No parameters
 * \returns No return value
 */
void UART_disable(void);

/**
 * \brief Aktiviert den UART
 * \param No parameters
 * \returns No return value
 */
void UART_enable(void);

/**
 * \brief Output a single character on the UART
 * \param unsigned char c: The character to be sent
 * \returns No return value
 */
void UART_putc(unsigned char c);

/**
 * \brief Output a string on the UART
 * \param unsigned char *s: pointer to string to be sent
 * \returns No return value
 */
void UART_puts(unsigned char *s);

/**
 * \brief Output a formatted 16 bit integer on the UART
 * \param uint16_t d: Value to be sent
 * \returns No return value
 */
void UART_putd(int16_t d);

void UART_putd_32(int32_t);

void UART_puth(int16_t d);

void UART_print_ubrr_vals(void);

void UART_main_test(void);


#endif /*UART_H_*/
