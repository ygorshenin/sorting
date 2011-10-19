#ifndef GENERATORS_GENERATOR_INTERFACE_H
#define GENERATORS_GENERATOR_INTERFACE_H

#include "boost/utility.hpp"


namespace generators {

template<typename T>
class GeneratorInterace: boost::noncopyable {
 public:
  GeneratorInterace() {}

  virtual void Generate(size_t size, T *buffer) = 0;
}; // class GeneratorInterace

}  // namespace generators

#endif // #ifndef GENERATORS_GENERATOR_INTERFACE_H
