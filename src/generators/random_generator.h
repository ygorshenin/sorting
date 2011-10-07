#ifndef GENERATORS_RANDOM_GENERATOR_H
#define GENERATORS_RANDOM_GENERATOR_H

#include <cstdlib>

#include "base/macros.h"
#include "generators/generator_interface.h"


namespace generators {

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

}  // namespace generators

#endif // #ifndef GENERATORS_RANDOM_GENERATOR_H
