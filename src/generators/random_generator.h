#ifndef GENERATORS_RANDOM_GENERATOR_H
#define GENERATORS_RANDOM_GENERATOR_H

#include <stdlib.h>

#include <algorithm>

#include "base/vector.h"
#include "generators/generator_interface.h"


namespace generators {

template<typename T>
class RandomGenerator: public GeneratorInterace<T> {
 public:
  virtual void Generate(T *object);
}; // class RandomGenerator

template<typename T>
class RandomGenerator<T*>: public GeneratorInterace<T*> {
 public:
  virtual void Generate(T **object) {
    plain_generator_.Generate(*object);
  }

 private:
  RandomGenerator<T> plain_generator_;
}; // class RandomGenerator

template<>
class RandomGenerator<int>: public GeneratorInterace<int> {
 public:
  virtual void Generate(int *d) {
      *d = rand();
  }
}; // class RandomGenerator

template<size_t N>
class RandomGenerator<base::Vector<N, int> >:
  public GeneratorInterace<base::Vector<N, int> > {
 public:
  virtual void Generate(base::Vector<N, int> *v) {
    for (size_t i = 0; i < N; ++i)
      (*v)[i] = rand();
  }
}; // class RandomGenerator

}  // namespace generators

#endif // #ifndef GENERATORS_RANDOM_GENERATOR_H
