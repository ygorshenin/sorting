#ifndef BASE_VECTOR_H
#define BASE_VECTOR_H

#include <functional>


namespace base {

template<size_t N, typename T>
class Vector {
 public:
  T& operator [] (size_t index) {
    return buffer_[index];
  }

  const T& operator [] (size_t index) const {
    return buffer_[index];
  }

 private:
  T buffer_[N];
}; // class Vector

}  // namespace base

#endif // #ifndef BASE_VECTOR_H
