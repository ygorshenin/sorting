#include <cassert>

#include <algorithm>

#include "base/macros.h"
#include "base/timer.h"
#include "generators/generator_interface.h"
#include "generators/random_generator.h"
#include "sort/sorter_interface.h"
#include "sort/stl_sorters.h"

using namespace base;
using namespace generators;
using namespace sort;


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
  SorterInterface<int> *sorter = new StlBasicSorter<int>();
  GeneratorInterace<int> *generator = new RandomGenerator<int>();
  delete sorter;
  delete generator;
  return 0;
}
