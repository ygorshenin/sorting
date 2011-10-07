#ifndef GENERATORS_GENERATOR_INTERFACE_H
#define GENERATORS_GENERATOR_INTERFACE_H

#include "base/macros.h"


namespace generators {

template<typename T>
class GeneratorInterace {
 public:
  GeneratorInterace() {}

  virtual void Generate(size_t size, T *buffer) = 0;

 private:
  DISABLE_EVIL_CONSTRUCTORS(GeneratorInterace);
}; // class GeneratorInterace

}  // namespace generators

#endif // #ifndef GENERATORS_GENERATOR_INTERFACE_H
