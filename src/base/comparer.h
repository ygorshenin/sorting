#ifndef BASE_COMPARER_H
#define BASE_COMPARER_H

#include "base/vector.h"


namespace base {

class PtrIntComparer {
 public:
  bool operator() (int *u, int *v) const {
    return *u < *v;
  }
}; // class PtrIntComparer


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

class PlainVectorComparer {
 public:
  template<size_t N, typename T>
  bool operator () (const Vector<N, T> &lhs, const Vector<N, T> &rhs) const {
    return MetaVectorComparer<N, 0, T>::Compare(lhs, rhs);
  }
}; // class PlainVectorComparer

class PtrVectorComparer {
 public:
  template<size_t N, typename T>
  bool operator () (const Vector<N, T> *lhs, const Vector<N, T> *rhs) const {
    return MetaVectorComparer<N, 0, T>::Compare(*lhs, *rhs);
  }
}; // class PtrVectorComparer

}  // namespace

#endif // #ifndef BASE_COMPARER_H
