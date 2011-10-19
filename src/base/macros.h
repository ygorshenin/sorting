#ifndef BASE_MACROS_H
#define BASE_MACROS_H

#include <cassert>


#define CHECK_EQ(expected, current) assert(expected == current)

#define CHECK_GE(expected, current) assert(current >= expected)


#endif // #ifndef BASE_MACROS_H
