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

template<size_t N, size_t I, typename T>
class MetaVectorComparer {
 public:
  static bool Compare(const Vector<N, T> &lhs, const Vector<N, T> &rhs) {
    return lhs[I] < rhs[I] ||
      (lhs[I] == rhs[I] && MetaVectorComparer<N, I + 1, T>::Compare(lhs, rhs));
  }
}; // class MetaVectorComparer

template<size_t N, typename T>
class MetaVectorComparer<N, N, T> {
 public:
  static bool Compare(const Vector<N, T> &lhs, const Vector<N, T> &rhs) {
    return false;
  }
}; // class MetaVectorComparer

class VectorComparer {
 public:
  template<size_t N, typename T>
  bool operator () (const Vector<N, T> &lhs, const Vector<N, T> &rhs) const {
    return MetaVectorComparer<N, 0, T>::Compare(lhs, rhs);
  }

  template<size_t N, typename T>
  bool operator () (const Vector<N, T> *lhs, const Vector<N, T> *rhs) const {
    return MetaVectorComparer<N, 0, T>::Compare(*lhs, *rhs);
  }
}; // class VectorComparer

}  // namespace base

#endif // #ifndef BASE_VECTOR_H
