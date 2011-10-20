#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "boost/chrono.hpp"
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "boost/ptr_container/ptr_vector.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/timer.hpp"

#include "base/macros.h"
#include "base/vector.h"
#include "generators/generator_interface.h"
#include "generators/random_generator.h"
#include "sorters/insertion_sorter.h"
#include "sorters/multithreaded_sorters.h"
#include "sorters/sorter_interface.h"
#include "sorters/stl_sorters.h"


using namespace base;
using namespace generators;
using namespace sorters;
using namespace std;

namespace filesystem = boost::filesystem;
namespace program_options = boost::program_options;

typedef void (*TesterMethod) ();

const int kMaxNumDimensions = 16;

bool FLAGS_use_insertion_sort;
int FLAGS_max_power;
int FLAGS_seed;
int FLAGS_num_dimensions;
string FLAGS_output_directory;


namespace {

template<typename T, typename Comparer>
double TestSortingAlgorithm(size_t size, T *data,
			    SorterInterface<T, Comparer> &sorter) {
  boost::chrono::system_clock::time_point start =
    boost::chrono::system_clock::now();
  sorter.Sort(size, data);
  boost::chrono::system_clock::time_point end =
    boost::chrono::system_clock::now();
  double elapsed_time = boost::chrono::duration<double>(end - start).count();

  Comparer comparer;
  for (size_t i = 0; i + 1 < size; ++i)
    assert(!comparer(data[i + 1], data[i]));

  return elapsed_time;
}

template<typename T>
void AllocateBuffer(size_t size, T **buffer) {
  *buffer = new T [size];
}

template<typename T>
void AllocateBuffer(size_t size, T ***buffer) {
  *buffer = new T* [size];
  for (size_t i = 0; i < size; ++i)
    (*buffer)[i] = new T();
}

template<typename T>
void DeallocateBuffer(T *buffer, size_t size) {
  delete [] buffer;
}

template<typename T>
void DeallocateBuffer(T **buffer, size_t size) {
  for (size_t i = 0; i < size; ++i)
    delete buffer[i];
  delete [] buffer;
}

template<typename T, typename Comparer>
void TwoPowerTesting(size_t max_power,
		     GeneratorInterace<T> *generator,
		     boost::ptr_vector<SorterInterface<T, Comparer> > &sorters,
		     vector<size_t> *test_size,
		     vector<vector<double> > *elapsed_time) {
  test_size->reserve(max_power);
  elapsed_time->resize(sorters.size());
  for (size_t cur_sorter = 0; cur_sorter < sorters.size(); ++cur_sorter)
    (*elapsed_time)[cur_sorter].resize(max_power);

  for (size_t power = 0; power < max_power; ++power) {
    const size_t size = 1 << power;

    test_size->push_back(size);

    T *data, *buffer;
    AllocateBuffer(size, &data);
    AllocateBuffer(size, &buffer);

    generator->Generate(size, data);

    for (size_t cur_sorter = 0; cur_sorter < sorters.size(); ++cur_sorter) {
      std::copy(data, data + size, buffer);
      (*elapsed_time)[cur_sorter][power] =
	TestSortingAlgorithm(size, buffer, sorters[cur_sorter]);
    }

    DeallocateBuffer(data, size);
    DeallocateBuffer(buffer, size);
  }
}

void DumpStatistic(const string &out_dir,
		   const vector<string> &sorters_names,
		   const vector<size_t> &test_size,
		   const vector<vector<double> > &elapsed_time) {
  const size_t n = sorters_names.size(), m = test_size.size();

  CHECK_EQ(n, sorters_names.size());
  CHECK_EQ(n, elapsed_time.size());

  filesystem::path output_directory(out_dir);

  for (size_t i = 0; i < sorters_names.size(); ++i) {
    CHECK_EQ(m, elapsed_time[i].size());

    filesystem::path current_path = output_directory /
      (sorters_names[i] + ".dat");
    ofstream ofs(current_path.c_str());
    assert(ofs);

    ofs << setprecision(6) << fixed;
    for (size_t j = 0; j < m; ++j)
      ofs << test_size[j] << "\t" << elapsed_time[i][j] << endl;
  }
}

template<typename T, typename Comparer>
void TestSortingAlgorithms() {
  boost::scoped_ptr<GeneratorInterace<T> > generator(new RandomGenerator<T>());

  boost::ptr_vector<SorterInterface<T, Comparer> > sorters;
  vector<string> sorters_names;

  sorters.push_back(new StlBasicSorter<T, Comparer>());
  sorters_names.push_back("stl_basic_sorter");

  sorters.push_back(new StlStableSorter<T, Comparer>());
  sorters_names.push_back("stl_stable_sorter");

  sorters.push_back(new StlHeapSorter<T, Comparer>());
  sorters_names.push_back("stl_heap_sorter");

  sorters.push_back(new StlPartitionSorter<T, Comparer>());
  sorters_names.push_back("stl_partition_sorter");

  sorters.push_back(new StlInplacePartitionSorter<T, Comparer>());
  sorters_names.push_back("stl_inplace_partition_sorter");

  sorters.push_back(new MultithreadedRandomizedQuickSorter<T, Comparer>(0));
  sorters_names.push_back("multithreaded_randomized_quick_sorter_0");

  sorters.push_back(new MultithreadedRandomizedQuickSorter<T, Comparer>(2));
  sorters_names.push_back("multithreaded_randomized_quick_sorter_2");

  sorters.push_back(new MultithreadedRandomizedQuickSorter<T, Comparer>(4));
  sorters_names.push_back("multithreaded_randomized_quick_sorter_4");

  sorters.push_back(new MultithreadedRandomizedQuickSorter<T, Comparer>(8));
  sorters_names.push_back("multithreaded_randomized_quick_sorter_8");

  if (FLAGS_use_insertion_sort) {
    sorters.push_back(new InsertionSorter<T, Comparer>());
    sorters_names.push_back("insertion_sorter");
  }

  vector<size_t> test_size;
  vector<vector<double> > elapsed_time;

  TwoPowerTesting(FLAGS_max_power, generator.get(), sorters,
		  &test_size, &elapsed_time);
  DumpStatistic(FLAGS_output_directory, sorters_names, test_size, elapsed_time);
}

template<size_t N, size_t I, typename T>
class MetaFillMethodsTable {
public:
  static void FillTable(TesterMethod *methods) {
    methods[I] = &TestSortingAlgorithms<Vector<I, T>, VectorComparer>;
    MetaFillMethodsTable<N, I + 1, T>::FillTable(methods);
  }
}; // class MetaFillMethodsTable

template<size_t N, typename T>
class MetaFillMethodsTable<N, N, T> {
public:
  static void FillTable(TesterMethod *methods) {
  }
}; // class MetaFillMethodsTable

template<size_t N, typename T>
void FillMethodsTable(TesterMethod *methods) {
  if (N > 0) {
    methods[0] = &TestSortingAlgorithms<int, less<int> >;
    MetaFillMethodsTable<N, 1, T>::FillTable(methods);
  }
}

}  // namespace

int main(int argc, char **argv) {
  program_options::options_description description("Allowed options");
  description.add_options()
    ("help,h", "produce help message")
    ("use_insertion_sort",
     program_options::value<bool>(&FLAGS_use_insertion_sort)->default_value(false),
     "use slow sort in tests")
    ("max_power,m",
     program_options::value<int>(&FLAGS_max_power)->default_value(24),
     "maximum power of two that will be used as maximum test size")
    ("seed,s",
     program_options::value<int>(&FLAGS_seed)->default_value(0),
     "seed for random generator, if zero, time is used as seed")
    ("num_dimensions,n",
     program_options::value<int>(&FLAGS_num_dimensions)->default_value(0),
     "number of vector dimensions, if zero, plain ints will be sorted")
    ("output_directory,o",
     program_options::value<string>(&FLAGS_output_directory)->default_value("out"),
     "output directory for storing test info")
    ;
  program_options::variables_map vm;
  program_options::store(program_options::
			 parse_command_line(argc, argv, description), vm);
  program_options::notify(vm);

  if (vm.count("help")) {
    cout << description << endl;
    return 1;
  }

  assert(FLAGS_output_directory != "");
  assert(FLAGS_max_power >= 0);

  if (FLAGS_seed == 0)
    srand(time(NULL));
  else
    srand(FLAGS_seed);

  filesystem::path output_directory(FLAGS_output_directory);
  if (filesystem::exists(output_directory))
    assert(is_directory(output_directory));
  else
    assert(filesystem::create_directory(FLAGS_output_directory));
  assert(FLAGS_num_dimensions >= 0);
  assert(FLAGS_num_dimensions < kMaxNumDimensions);

  TesterMethod methods[kMaxNumDimensions];
  FillMethodsTable<kMaxNumDimensions, int>(methods);
  (*methods[FLAGS_num_dimensions])();
  return 0;
}
