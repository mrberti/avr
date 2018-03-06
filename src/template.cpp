
#include "template.hpp"

template <typename T>
T test_template_temp(T a)
{
  return a*a;
}

char _test_template_char(char a)
{
  a = test_template_temp<char>(a);
  return a;
}

int _test_template_int(int a)
{
  a = test_template_temp<int>(a);
  return a;
}

extern "C" int test_template_int(int a)
{
  a = _test_template_int(a);
  return a;
}

extern "C" char test_template_char(char a)
{
  a = _test_template_char(a);
  return a;
}
