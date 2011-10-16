#ifndef GENERATORS_RANDOM_GENERATOR_H
#define GENERATORS_RANDOM_GENERATOR_H

#include <cstdlib>

#include "base/macros.h"
#include "base/vector.h"
#include "generators/generator_interface.h"


namespace generators {

using base::Vector;


template<typename T>
class RandomGenerator: public GeneratorInterace<T> {
 public:
  RandomGenerator() {}

  virtual void Generate(size_t size, T *buffer);

 private:
  DISABLE_EVIL_CONSTRUCTORS(RandomGenerator);
}; // class RandomGenerator

template<>
class RandomGenerator<int>: public GeneratorInterace<int> {
 public:
  RandomGenerator() {}

  virtual void Generate(size_t size, int *buffer) {
    for (size_t i = 0; i < size; ++i)
      buffer[i] = rand();
  }

 private:
   DISABLE_EVIL_CONSTRUCTORS(RandomGenerator);
}; // class RandomGenerator

template<size_t N>
class RandomGenerator<Vector<N, int> >:
  public GeneratorInterace<Vector<N, int> > {
 public:
  RandomGenerator() {}

  virtual void Generate(size_t size, Vector<N, int> *buffer) {
    for (size_t i = 0; i < size; ++i)
      for (size_t j = 0; j < N; ++j)
	buffer[i][j] = rand();
  }

 private:
  DISABLE_EVIL_CONSTRUCTORS(RandomGenerator);
}; // class RandomGenerator


}  // namespace generators

#endif // #ifndef GENERATORS_RANDOM_GENERATOR_H
