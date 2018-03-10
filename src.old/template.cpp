
#include "template.h"

#include <stdint.h>

//typedef int T;

template <typename T>
class test_class
{
public:
  T test(T a)
  {
    return a*2;
  }
};



test_class<char> A_char;

int C::f(int a)
{
  return a*a;
}

EXPORT_C int test_template_int(int a)
{
  int ret = a;
  return ret;
}

EXPORT_C char test_template_char(char a, C* c)
{
  //c = new C();
  return c->f(a);
}
