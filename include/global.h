#ifndef _GLOBAL_H
#define _GLOBAL_H

#ifndef F_CPU
#	define F_CPU 16000000UL
#endif //F_CPU

#ifdef __cplusplus
# define EXPORT_C extern "C"
#else
# define EXPORT_C
#endif

/* Set multiple bits while preserving other bits of the register */
#define SET_8BIT_REG(REG,MASK,VAL) (REG = (REG & ~(MASK)) | VAL)

#endif
