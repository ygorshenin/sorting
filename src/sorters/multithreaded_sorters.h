#ifndef MULTITHREADED_SORTERS_H
#define MULTITHREADED_SORTERS_H

#include <cstdio>

#include <functional>

#include <boost/thread/thread.hpp>


namespace sorters {

template<typename T, typename Comparer>
class MultithreadedRandomizedQuickSorter: public SorterInterface<T, Comparer> {
 public:
   MultithreadedRandomizedQuickSorter(size_t num_threads):
     num_threads_(num_threads) {
   }

   virtual void Sort(size_t size, T *objects) {
     Comparer comparer;

     SortImpl(size, objects, comparer, num_threads_);
   }

 private:
   static void Partition(size_t size, T *objects, Comparer &comparer,
		  size_t *left_bound, size_t *right_bound) {
     std::swap(objects[rand() % size], objects[size - 1]);
     T pivot = objects[size - 1];

     *left_bound = 0;
     for (size_t i = 0; i < size; ++i)
       if (comparer(objects[i], pivot)) {
	 std::swap(objects[*left_bound], objects[i]);
	 ++*left_bound;
       }

     *right_bound = *left_bound;
     for (size_t i = *left_bound; i < size; ++i)
       if (!comparer(pivot, objects[i])) {
	 std::swap(objects[*right_bound], objects[i]);
	 ++*right_bound;
       }
   }

   static void SortImpl(size_t size, T *objects, Comparer &comparer,
			size_t thread_limit) {
     if (size > 1) {
       if (thread_limit > 0) {
	 size_t left_bound, right_bound;
	 Partition(size, objects, comparer, &left_bound, &right_bound);

	 size_t left_threads = (thread_limit - 1) / 2;
	 size_t right_threads = thread_limit - 1 - left_threads;
	 boost::thread thread(&SortImpl, size - right_bound,
			      objects + right_bound, comparer, right_threads);
	 SortImpl(left_bound, objects, comparer, left_threads);
	 thread.join();
       } else
	 std::sort(objects, objects + size, comparer);
     }
   }


   size_t num_threads_;
}; // class MultithreadedRandomizedQuickSorter

}  // namespace sorters


#endif // #ifndef MULTITHREADED_SORTERS_H
