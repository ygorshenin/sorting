#ifndef SORTERS_SORTER_INTERFACE_H
#define SORTERS_SORTER_INTERFACE_H

#include "boost/utility.hpp"


namespace sorters {

template<typename T, typename Comparer>
class SorterInterface: boost::noncopyable {
  public:
    SorterInterface() {}

    virtual ~SorterInterface() {}

    virtual void Sort(size_t size, T *objects) = 0;
}; // class SorterInterface

}  // namespace sorters

#endif // #ifndef SORTERS_SORTER_INTERFACE_H
