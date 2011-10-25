#ifndef SORTERS_STL_SORTERS_H
#define SORTERS_STL_SORTERS_H

#include <stdlib.h>

#include <algorithm>
#include <functional>
#include <iostream>

#include "sorters/sorter_interface.h"

using std::clog;
using std::endl;


namespace sorters {

template<typename T, typename Comparer>
class StlBasicSorter: public SorterInterface<T, Comparer> {
  public:
    StlBasicSorter() {}

    virtual void Sort(size_t size, T *objects) {
      std::sort(objects, objects + size, Comparer());
    }
}; // class StlBasicSorter

template<typename T, typename Comparer>
class StlStableSorter: public SorterInterface<T, Comparer> {
  public:
    StlStableSorter() {}

    virtual void Sort(size_t size, T *objects) {
      std::stable_sort(objects, objects + size, Comparer());
    }
}; // class StlStableSorter

template<typename T, typename Comparer>
class StlHeapSorter: public SorterInterface<T, Comparer> {
  public:
    StlHeapSorter() {}

    virtual void Sort(size_t size, T *objects) {
      Comparer comparer;
      std::make_heap(objects, objects + size, comparer);
      std::sort_heap(objects, objects + size, comparer);
    }
}; // class StlHeapSorter

template<typename T, typename Comparer>
class StlPartitionSorter: public SorterInterface<T, Comparer> {
  public:
    StlPartitionSorter() {}

    virtual void Sort(size_t size, T *objects) {
      buffer_ = new (std::nothrow) T [EstimateBufferSize(size)];

      if (buffer_ == NULL) {
	clog << "StlPartitionSorter::Sort: can't allocate buffer" << endl;
	clog << "Terminating...";
	exit(-1);
      }

      free_position_ = 0;

      Comparer comparer;
      PartitionSort(size, objects, comparer);

      delete [] buffer_;
    }

  private:
    size_t EstimateBufferSize(size_t size) const {
      if (size < 2)
	return 0;
      size_t left_size = size / 2, right_size = size - left_size;
      return size +
	EstimateBufferSize(left_size) +
	EstimateBufferSize(right_size);
    }

    void PartitionSort(size_t size, T *objects, Comparer &comparer) {
      if (size < 2)
	return;
      size_t left_size = size / 2, right_size = size - left_size;

      T *left_buffer = buffer_ + free_position_;
      free_position_ += left_size;
      T *right_buffer = buffer_ + free_position_;
      free_position_ += right_size;

      std::copy(objects, objects + left_size, left_buffer);
      std::copy(objects + left_size, objects + size, right_buffer);

      PartitionSort(left_size, left_buffer, comparer);
      PartitionSort(right_size, right_buffer, comparer);

      std::merge(left_buffer, left_buffer + left_size,
		 right_buffer, right_buffer + right_size,
		 objects, comparer);
    }

    T *buffer_;
    size_t free_position_;
}; // class StlPartitionSorter

template<typename T, typename Comparer>
  class StlInplacePartitionSorter: public SorterInterface<T, Comparer> {
  public:
    StlInplacePartitionSorter() {}

    virtual void Sort(size_t size, T *objects) {
      Comparer comparer;
      PartitionSort(size, objects, comparer);
    }

  private:
    void PartitionSort(size_t size, T *objects, Comparer &comparer) {
      if (size < 2)
	return;

      size_t left_size = size / 2, right_size = size - left_size;
      PartitionSort(left_size, objects, comparer);
      PartitionSort(right_size, objects + left_size, comparer);

      std::inplace_merge(objects, objects + left_size, objects + size,
			 comparer);
    }
}; // class StlInplacePartitionSorter

}  // namespace sorters

#endif // #ifndef SORTERS_STL_SORTERS_H
