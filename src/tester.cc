#include <cassert>

#include <algorithm>

#include "base/macros.h"
#include "base/timer.h"
#include "generators/generator_interface.h"
#include "generators/random_generator.h"
#include "sorters/sorter_interface.h"
#include "sorters/stl_sorters.h"

using namespace base;
using namespace generators;
using namespace sorters;


template<typename T, template <typename> class Comparer>
double TestSortingAlgorithm(size_t size, T *data,
			    SorterInterface<T, Comparer> *sorter) {
  base::Timer timer;
  timer.Start();
  sorter->Sort(size, data);
  timer.Stop();

  double elapsed_time = timer.GetTimeElapsed();
  CHECK_GE(0.0, elapsed_time);

  return elapsed_time;
}

int main(int argc, char **argv) {
  SorterInterface<int> *sorter = new StlInplacePartitionSorter<int>();
  GeneratorInterace<int> *generator = new RandomGenerator<int>();

  const size_t kSize = 20000000;
  int *data = new int [kSize];
  generator->Generate(kSize, data);
  printf("Elapsed time: %.4lf\n", TestSortingAlgorithm(kSize, data, sorter));

  delete [] data;
  delete generator;
  delete sorter;
  return 0;
}
