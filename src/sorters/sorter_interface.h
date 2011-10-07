#ifndef SORTERS_SORTER_INTERFACE_H
#define SORTERS_SORTER_INTERFACE_H

#include <functional>

#include "base/macros.h"


namespace sorters {

template<typename T, template <typename> class Comparer = std::less >
class SorterInterface {
  public:
    SorterInterface() {}

    virtual ~SorterInterface() {}

    virtual void Sort(size_t size, T *objects) = 0;

  private:
    DISABLE_EVIL_CONSTRUCTORS(SorterInterface);
}; // class SorterInterface

}  // namespace sorters

#endif // #ifndef SORTERS_SORTER_INTERFACE_H
