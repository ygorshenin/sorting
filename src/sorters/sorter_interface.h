#ifndef SORT_SORTER_INTERFACE_H
#define SORT_SORTER_INTERFACE_H

#include <functional>

#include "base/macros.h"


namespace sort {

  template<typename T, template <typename> class Comparer = std::less >
class SorterInterface {
  public:
    SorterInterface() {}

    virtual ~SorterInterface() {}

    virtual void Sort(size_t size, T *objects) = 0;

  private:
    DISABLE_EVIL_CONSTRUCTORS(SorterInterface);
}; // class SorterInterface

}  // namespace sort

#endif // #ifndef SORT_SORTER_INTERFACE_H
