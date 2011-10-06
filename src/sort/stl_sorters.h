#ifndef SORT_STL_SORTERS_H
#define SORT_STL_SORTERS_H

#include <algorithm>

#include "base/macros.h"
#include "sort/sorter_interface.h"


namespace sort {

template<typename T, template <typename> class Comparer = std::less>
class StlBasicSorter: public SorterInterface<T, Comparer> {
  public:
    StlBasicSorter() {}

    virtual void Sort(size_t size, T *objects) {
      std::sort(objects, objects + size, Comparer<T>());
    }

  private:
    DISABLE_EVIL_CONSTRUCTORS(StlBasicSorter);
}; // class StlBasicSorter

template<typename T, template <typename> class Comparer = std::less>
class StlHeapSorter: public SorterInterface<T, Comparer> {
  public:
    StlHeapSorter() {}

    virtual void Sort(size_t size, T *objects) {
      Comparer<T> comparer;
      std::make_heap(objects, objects + size, comparer);
      std::sort_heap(objects, objects + size, comparer);
    }

  private:
    DISABLE_EVIL_CONSTRUCTORS(StlHeapSorter);
}; // class StlHeapSorter

template<typename T, template <typename> class Comparer = std::less>
class StlPartitionSorter: public SorterInterface<T, Comparer> {
  public:
    StlPartitionSorter() {}

    virtual void Sort(size_t size, T *objects) {
      Comparer<T> comparer;
      PartitionSort(size, objects, comparer);
    }

  private:
    void PartitionSort(size_t size, T *objects, Comparer<T> &comparer) {
      if (size < 2)
	return;
      size_t left_size = size / 2, right_size = size - left_size;
      T *left_buffer = new T [left_size];
      T *right_buffer = new T [right_size];

      copy(objects, objects + left_size, left_buffer);
      copy(objects + left_size, objects + size, right_buffer);

      PartitionSort(left_size, left_buffer, comparer);
      PartitionSort(right_size, right_buffer, comparer);

      std::merge(left_buffer, left_buffer + left_size,
		 right_buffer, right_buffer + right_size,
		 objects, comparer);

      delete [] left_buffer;
      delete [] right_buffer;
    }

    DISABLE_EVIL_CONSTRUCTORS(StlPartitionSorter);
}; // class StlPartitionSorter

template<typename T, template <typename> class Comparer = std::less>
class StlInplacePartitionSorter: public SorterInterface<T, Comparer> {
  public:
    StlInplacePartitionSorter() {}

    virtual void Sort(size_t size, T *objects) {
      Comparer<T> comparer;
      PartitionSort(size, objects, comparer);
    }

  private:
    void PartitionSort(size_t size, T *objects, Comparer<T> &comparer) {
      if (size < 2)
	return;

      size_t left_size = size / 2, right_size = size - left_size;
      PartitionSort(left_size, objects, comparer);
      PartitionSort(right_size, objects + left_size, comparer);

      std::inplace_merge(objects, objects + left_size, objects + size,
			 comparer);
    }

    DISABLE_EVIL_CONSTRUCTORS(StlInplacePartitionSorter);
}; // class StlInplacePartitionSorter

}  // namespace sort

#endif // #ifndef SORT_STL_SORTERS_H
