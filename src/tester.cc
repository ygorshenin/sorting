#include <assert.h>
#include <errno.h>
#include <sys/stat.h>


#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "base/macros.h"
#include "base/timer.h"
#include "generators/generator_interface.h"
#include "generators/random_generator.h"
#include "sorters/sorter_interface.h"
#include "sorters/stl_sorters.h"
#include "sorters/insertion_sorter.h"

#include <gflags/gflags.h>

using namespace std;

using namespace base;
using namespace generators;
using namespace sorters;


DEFINE_bool(use_insertion_sort, false, "use insertion sort in tests");
DEFINE_int32(max_power, 24, "maximum power of two that will be used as test size");
DEFINE_string(out_dir, "out", "output directory for statistics");


template<typename T, template <typename> class Comparer>
double TestSortingAlgorithm(size_t size, T *data,
			    SorterInterface<T, Comparer> *sorter) {
  base::Timer timer;

  timer.Start();
  sorter->Sort(size, data);
  timer.Stop();

  Comparer<T> comparer;
  for (size_t i = 0; i + 1 < size; ++i)
    assert(!comparer(data[i + 1], data[i]));

  double elapsed_time = timer.GetTimeElapsed();
  CHECK_GE(0.0, elapsed_time);

  return elapsed_time;
}

template<typename T, template <typename> class Comparer>
void TwoPowerTesting(size_t max_power,
		     GeneratorInterace<T> *generator,
		     vector<SorterInterface<T, Comparer> *> &sorters,
		     vector<size_t> *test_size,
		     vector<vector<double> > *elapsed_time) {
  test_size->reserve(max_power);
  elapsed_time->resize(sorters.size());
  for (size_t cur_sorter = 0; cur_sorter < sorters.size(); ++cur_sorter)
    (*elapsed_time)[cur_sorter].resize(max_power);

  for (size_t power = 0; power < max_power; ++power) {
    const size_t size = 1 << power;

    test_size->push_back(size);

    T *data = new T [size], *buffer = new T [size];
    generator->Generate(size, data);

    for (size_t cur_sorter = 0; cur_sorter < sorters.size(); ++cur_sorter) {
      std::copy(data, data + size, buffer);
      (*elapsed_time)[cur_sorter][power] =
	TestSortingAlgorithm(size, buffer, sorters[cur_sorter]);
    }

    delete [] data;
    delete [] buffer;
  }
}

void DumpStatistic(const string &out_dir,
		   const vector<string> &sorters_names,
		   const vector<size_t> &test_size,
		   const vector<vector<double> > &elapsed_time) {
  const size_t n = sorters_names.size(), m = test_size.size();

  CHECK_EQ(n, sorters_names.size());
  CHECK_EQ(n, elapsed_time.size());

  for (size_t i = 0; i < sorters_names.size(); ++i) {
    CHECK_EQ(m, elapsed_time[i].size());

    const string path = out_dir + "/" + sorters_names[i] + ".dat";
    ofstream ofs(path.c_str());

    ofs << setprecision(6) << fixed;
    for (size_t j = 0; j < m; ++j)
      ofs << test_size[j] << "\t" << elapsed_time[i][j] << endl;
  }
}


int main(int argc, char **argv) {
  string usage = "This program measures performance of different sorting algorithms\n";
  usage += "Sample usage: ";
  usage += argv[0];
  usage += " --max_power=20\n";

  google::SetUsageMessage(usage);
  google::ParseCommandLineFlags(&argc, &argv, true);

  assert(FLAGS_out_dir != "");
  assert(FLAGS_max_power >= 0);

  if (mkdir(FLAGS_out_dir.c_str(), 0777) != 0 && errno != EEXIST) {
    perror("Can't create output directory");
    return 1;
  }

  GeneratorInterace<int> *generator = new RandomGenerator<int>();

  vector<SorterInterface<int> *> sorters;
  vector<string> sorters_names;

  sorters.push_back(new StlBasicSorter<int>());
  sorters_names.push_back("stl_basic_sorter");

  sorters.push_back(new StlHeapSorter<int>());
  sorters_names.push_back("stl_heap_sorter");

  sorters.push_back(new StlPartitionSorter<int>());
  sorters_names.push_back("stl_partition_sorter");

  sorters.push_back(new StlInplacePartitionSorter<int>());
  sorters_names.push_back("stl_inplace_partition_sorter");

  if (FLAGS_use_insertion_sort) {
    sorters.push_back(new InsertionSorter<int>());
    sorters_names.push_back("insertion_sorter");
  }

  vector<size_t> test_size;
  vector<vector<double> > elapsed_time;

  TwoPowerTesting(FLAGS_max_power, generator, sorters,
		  &test_size, &elapsed_time);
  DumpStatistic(FLAGS_out_dir, sorters_names, test_size, elapsed_time);

  delete generator;
  for (size_t cur_sorter = 0; cur_sorter < sorters.size(); ++cur_sorter)
    delete sorters[cur_sorter];
  return 0;
}
