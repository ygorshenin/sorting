#include <cassert>

#include <algorithm>

#include "base/macros.h"
#include "sort/sorter_interface.h"
#include "sort/stl_sorters.h"
#include "base/timer.h"


template<typename T, template <typename> class Comparer>
double TestSortingAlgorithm(size_t size, T *data,
			    sort::SorterInterface<T, Comparer> *sorter) {
  base::Timer timer;
  timer.Start();
  sorter->Sort(size, data);
  timer.Stop();

  double elapsed_time = timer.GetTimeElapsed();
  CHECK_GE(0.0, elapsed_time);

  return elapsed_time;
}

int main(int argc, char **argv) {
  return 0;
}
