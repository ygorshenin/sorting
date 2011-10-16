#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <gflags/gflags.h>

#include "base/macros.h"
#include "base/timer.h"
#include "base/vector.h"
#include "generators/generator_interface.h"
#include "generators/random_generator.h"
#include "sorters/insertion_sorter.h"
#include "sorters/sorter_interface.h"
#include "sorters/stl_sorters.h"


using namespace std;

using namespace base;
using namespace generators;
using namespace sorters;


typedef void (*TesterMethod) ();

const int kMaxNumDimensions = 16;

DEFINE_bool(use_insertion_sort, false, "Use insertion sort in tests.");
DEFINE_int32(max_power, 24, "Maximum power of two that will be used \
                             as test size.");
DEFINE_int32(seed, 0, "seed for random generator. If zero, time is used \
                       as seed.");
DEFINE_int32(num_dimensions, 0, "Number of vector dimensions, from 0 to 15, \
                                 inclusively. \
                                 If zero, plain ints will be sorted.");
DEFINE_string(out_dir, "out", "Output directory for statistics.");


namespace {

template<typename T, typename Comparer>
double TestSortingAlgorithm(size_t size, T *data,
			    SorterInterface<T, Comparer> *sorter) {
  base::Timer timer;

  timer.Start();
  sorter->Sort(size, data);
  timer.Stop();

  Comparer comparer;
  for (size_t i = 0; i + 1 < size; ++i)
    assert(!comparer(data[i + 1], data[i]));

  double elapsed_time = timer.GetTimeElapsed();
  CHECK_GE(0.0, elapsed_time);

  return elapsed_time;
}

template<typename T, typename Comparer>
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

template<typename T, typename Comparer>
void TestSortingAlgorithms() {
  GeneratorInterace<T> *generator = new RandomGenerator<T>();

  vector<SorterInterface<T, Comparer> *> sorters;
  vector<string> sorters_names;

  sorters.push_back(new StlBasicSorter<T, Comparer>());
  sorters_names.push_back("stl_basic_sorter");

  sorters.push_back(new StlHeapSorter<T, Comparer>());
  sorters_names.push_back("stl_heap_sorter");

  sorters.push_back(new StlPartitionSorter<T, Comparer>());
  sorters_names.push_back("stl_partition_sorter");

  sorters.push_back(new StlInplacePartitionSorter<T, Comparer>());
  sorters_names.push_back("stl_inplace_partition_sorter");

  if (FLAGS_use_insertion_sort) {
    sorters.push_back(new InsertionSorter<T, Comparer>());
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
}

template<size_t N, size_t I>
class MetaFillMethodsTable {
public:
  static void FillTable(TesterMethod *methods) {
    methods[I] = &TestSortingAlgorithms<Vector<I, int>,
					VectorComparer<I, int> >;
    MetaFillMethodsTable<N, I + 1>::FillTable(methods);
  }
}; // class MetaFillMethodsTable

template<size_t N>
class MetaFillMethodsTable<N, N> {
public:
  static void FillTable(TesterMethod *methods) {
  }
}; // class MetaFillMethodsTable

template<size_t N>
void FillMethodsTable(TesterMethod *methods) {
  if (N > 0) {
    methods[0] = &TestSortingAlgorithms<int, less<int> >;
    MetaFillMethodsTable<N, 1>::FillTable(methods);
  }
}

}  // namespace

int main(int argc, char **argv) {
  string usage = "This program measures performance of \
                  different sorting algorithms\n";
  usage += "Sample usage: ";
  usage += argv[0];
  usage += " --max_power=20\n";

  google::SetUsageMessage(usage);
  google::ParseCommandLineFlags(&argc, &argv, true);

  assert(FLAGS_out_dir != "");
  assert(FLAGS_max_power >= 0);

  if (FLAGS_seed == 0)
    srand(time(NULL));
  else
    srand(FLAGS_seed);

  if (mkdir(FLAGS_out_dir.c_str(), 0777) != 0 && errno != EEXIST) {
    perror("Can't create output directory");
    return 1;
  }

  assert(FLAGS_num_dimensions >= 0);
  assert(FLAGS_num_dimensions < kMaxNumDimensions);

  TesterMethod methods[kMaxNumDimensions];
  FillMethodsTable<kMaxNumDimensions>(methods);
  (*methods[FLAGS_num_dimensions])();
  return 0;
}
