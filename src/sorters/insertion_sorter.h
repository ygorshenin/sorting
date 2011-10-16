#ifndef SORTERS_INSERTION_SORTER_H
#define SORTERS_INSERTION_SORTER_H

#include <functional>

#include "base/macros.h"
#include "sorters/sorter_interface.h"


namespace sorters {

template<typename T, typename Comparer>
class InsertionSorter: public SorterInterface<T, Comparer> {
 public:
   InsertionSorter() {}

   virtual void Sort(size_t size, T *objects) {
     Comparer comparer;

     size_t i = 1, j;
     while (i < size) {
       T current(objects[i]);
       j = i;

       while (j > 0 && !comparer(objects[j - 1], current)) {
	 objects[j] = objects[j - 1];
	 --j;
       }
       objects[j] = current;

       ++i;
     }
   }
}; // class InsertionSorter

}  // namespace sorters

#endif // #ifndef SORTERS_INSERTION_SORTER_H
