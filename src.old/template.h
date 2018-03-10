#ifndef TEMPLATE_H
#define TEMPLATE_H

/*
 * Redefine function declarations here, so that other C sources can
 * call.
 */
#ifdef __cplusplus

# define EXPORT_C extern "C"

class C
{
public:
  volatile int lock;
  int f(int a);
};

#else

# define EXPORT_C
typedef struct C_s C;

#endif

#include <stdint.h>

EXPORT_C int test_template_int(int a);
EXPORT_C char test_template_char(char a, C* c);

#endif
