#ifndef _GLOBAL_H
#define _GLOBAL_H

#ifndef F_CPU
#	define F_CPU 16000000UL
#endif //F_CPU

/* Set multiple bits while preserving other bits of the register */
#define SET_8BIT_REG(REG,MASK,VAL) (REG = (REG & ~(MASK)) | VAL)

#endif
