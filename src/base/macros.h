#ifndef BASE_MACROS_H
#define BASE_MACROS_H

#include <cassert>


#define CHECK_GE(expected, current) assert(current >= expected)

#define DISABLE_EVIL_CONSTRUCTORS(ClassName) \
  ClassName(const ClassName&);		     \
  ClassName& operator = (const ClassName&)


#endif // #ifndef BASE_MACROS_H
