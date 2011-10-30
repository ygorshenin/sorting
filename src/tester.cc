#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <new>
#include <string>
#include <vector>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "boost/ptr_container/ptr_vector.hpp"
#include "boost/scoped_ptr.hpp"

#include "base/comparer.h"
#include "base/macros.h"
#include "base/timer.h"
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

const int kMaxNumDimensions = 8;

bool FLAGS_use_insertion_sort;
bool FLAGS_sort_pointers;
int FLAGS_max_power;
int FLAGS_seed;
int FLAGS_num_dimensions;
string FLAGS_output_directory;


namespace {

struct InfoEntry {
  size_t test_size_;
  double generating_time_;
  double sorting_time_;
  double checking_time_;
}; // struct InfoEntry

ostream& operator << (ostream& os, const InfoEntry& entry) {
  os << setprecision(6) << fixed;

  os << "Test size: " << entry.test_size_ << endl;
  os << "Generating time: " << entry.generating_time_ << endl;
  os << "Sorting time: " << entry.sorting_time_ << endl;
  os << "Checking time: " << entry.checking_time_ << endl;

  return os;
}

template<typename T, typename Comparer>
void TestSortingAlgorithm(size_t size, T *data,
			  SorterInterface<T, Comparer> &sorter,
			  InfoEntry &entry) {
  Timer timer;

  sorter.Sort(size, data);
  entry.sorting_time_ = timer.Elapsed();

  Comparer comparer;

  timer.Restart();
  for (size_t i = 0; i + 1 < size; ++i)
    assert(!comparer(data[i + 1], data[i]));
  entry.checking_time_ = timer.Elapsed();
}

template<typename T>
void AllocateBuffer(size_t size, T **buffer) {
  *buffer = new (std::nothrow) T [size];
  if (*buffer == NULL) {
    clog << "AllocateBuffer: can't allocate buffer" << endl;
    clog << "Terminating..." << endl;
    exit(-1);
  }
}

template<typename T>
void AllocateBuffer(size_t size, T ***buffer) {
  *buffer = new (std::nothrow) T* [size];
  if (*buffer == NULL) {
    clog << "AllocateBuffer: can't allocate buffer" << endl;
    clog << "Terminating..." << endl;
    exit(-1);
  }

  for (size_t i = 0; i < size; ++i) {
    (*buffer)[i] = new (std::nothrow) T();
    if ((*buffer)[i] == NULL) {
      clog << "AllocateBuffer: can't instaniate object" << endl;
      clog << "Terminating..." << endl;
      exit(-1);
    }
  }
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
void TwoPowerTesting(GeneratorInterace<T> *generator,
		     boost::ptr_vector<SorterInterface<T, Comparer> > &sorters,
		     vector<vector<InfoEntry> > *info) {
  info->resize(sorters.size());

  for (size_t cur_sorter = 0; cur_sorter < sorters.size(); ++cur_sorter)
    (*info)[cur_sorter].resize(FLAGS_max_power + 1);

  Timer timer;

  for (int power = 0; power <= FLAGS_max_power; ++power) {
    const size_t size = 1 << power;

    T *data, *buffer;
    AllocateBuffer(size, &data);
    buffer = new T [size];

    timer.Restart();
    for (size_t i = 0; i < size; ++i)
      generator->Generate(&data[i]);
    double generating_time = timer.Elapsed();

    for (size_t cur_sorter = 0; cur_sorter < sorters.size(); ++cur_sorter) {
      (*info)[cur_sorter][power].test_size_ = size;
      (*info)[cur_sorter][power].generating_time_ = generating_time;

      std::copy(data, data + size, buffer);
      TestSortingAlgorithm(size, buffer, sorters[cur_sorter],
			   (*info)[cur_sorter][power]);
    }

    DeallocateBuffer(data, size);
    delete [] buffer;
  }
}

void DumpStatistic(const string &out_dir,
		   const vector<string> &sorters_names,
		   const vector<vector<InfoEntry> > &info) {
  if (info.empty())
    return;

  const size_t n = sorters_names.size(), m = info.front().size();

  CHECK_EQ(n, sorters_names.size());
  CHECK_EQ(n, info.size());

  filesystem::path output_directory(out_dir);

  for (size_t i = 0; i < sorters_names.size(); ++i) {
    CHECK_EQ(m, info[i].size());

    {
      filesystem::path current_path = output_directory /
	(sorters_names[i] + ".dat");
      ofstream ofs(current_path.c_str());
      assert(ofs);

      ofs << setprecision(6) << fixed;
	for (size_t j = 0; j < m; ++j)
	  ofs <<
	    info[i][j].test_size_ << '\t' <<
	    info[i][j].sorting_time_ << endl;
    }

    {
      filesystem::path current_path = output_directory /
	(sorters_names[i] + ".log");
      ofstream ofs(current_path.c_str());
      assert(ofs);

      copy(info[i].begin(), info[i].end(),
	   ostream_iterator<InfoEntry>(ofs, "\n"));
    }
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

  vector<vector<InfoEntry> > info;

  TwoPowerTesting(generator.get(), sorters, &info);
  DumpStatistic(FLAGS_output_directory, sorters_names, info);
}

template<size_t N, size_t I>
class MetaFillMethodsTable {
public:
  static void FillTable(TesterMethod *plain_methods,
			TesterMethod *ptr_methods) {
    plain_methods[I] = &TestSortingAlgorithms<Vector<I, int>,
					      PlainVectorComparer>;
    ptr_methods[I] = &TestSortingAlgorithms<Vector<I, int>*,
					    PtrVectorComparer>;
    MetaFillMethodsTable<N, I + 1>::FillTable(plain_methods, ptr_methods);
  }
}; // class MetaFillMethodsTable

template<size_t N>
class MetaFillMethodsTable<N, N> {
public:
  static void FillTable(TesterMethod *plain_methods,
			TesterMethod *ptr_methods) {
  }
}; // class MetaFillMethodsTable

template<size_t N>
void FillMethodsTable(TesterMethod *plain_methods, TesterMethod *ptr_methods) {
  if (N > 0) {
    plain_methods[0] = &TestSortingAlgorithms<int, less<int> >;
    ptr_methods[0] = &TestSortingAlgorithms<int*, PtrIntComparer>;

    MetaFillMethodsTable<N, 1>::FillTable(plain_methods, ptr_methods);
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
    ("sort_pointers",
     program_options::value<bool>(&FLAGS_sort_pointers)->default_value(false),
     "sort pointers to objects instead of objects")
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
    FLAGS_seed = time(NULL);
  srand(FLAGS_seed);

  clog << "Maximum test size: " << (1 << FLAGS_max_power) << endl;
  if (FLAGS_sort_pointers) {
    if (FLAGS_num_dimensions == 0)
      clog << "Pointers to ints will be sorted" << endl;
    else
      clog << "Pointers to vectors of size " << FLAGS_num_dimensions <<
	" will be sorted" << endl;
  } else {
    if (FLAGS_num_dimensions == 0)
      clog << "Plain ints will be sorted" << endl;
    else
      clog << "Vectors of size " << FLAGS_num_dimensions <<
	" will be sorted" << endl;
  }
  clog << "Current seed: " << FLAGS_seed << endl;

  filesystem::path output_directory(FLAGS_output_directory);
  if (filesystem::exists(output_directory))
    assert(is_directory(output_directory));
  else
    assert(filesystem::create_directory(FLAGS_output_directory));
  assert(FLAGS_num_dimensions >= 0);
  assert(FLAGS_num_dimensions <= kMaxNumDimensions);

  TesterMethod plain_methods[kMaxNumDimensions], ptr_methods[kMaxNumDimensions];
  FillMethodsTable<kMaxNumDimensions + 1>(plain_methods, ptr_methods);

  if (!FLAGS_sort_pointers)
    (*plain_methods[FLAGS_num_dimensions])();
  else
    (*ptr_methods[FLAGS_num_dimensions])();
  return 0;
}
