#define FILE_OFFSET_BITS 64
#include "common.h"

#define VERIFS2_COPY_BENCH
#include "inode.hpp"
#include "file.hpp"
#include "directory.hpp"
#include "symlink.hpp"
#include "special_inode.hpp"

#include <cmath>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include <gperftools/profiler.h>

DEFINE_int32(inodes, 25600, "Number of inodes to generate");
DEFINE_int32(fsize_min, 0, "Minimum size of regular file (in bytes)");
DEFINE_int32(fsize_max, 65536, "Maximum size of regular file (in bytes)");
DEFINE_int32(dirent_min, 2, "Minimum number of children in each directory");
DEFINE_int32(dirent_max, 100, "Max number of children in each directory");

enum benchmark::TimeUnit display_time_unit;
std::string& toLower(std::string &str) {
  const int kDiff = 'a' - 'A';
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] >= 'A' && str[i] <= 'Z')
      str[i] -= kDiff;
  }
  return str;
}
bool validate_unit_option(const char *flagname, const std::string &value) {
  std::string val2 = value;
  toLower(val2);
  if (val2 == "ms" || val2 == "millisecond" || val2 == "milliseconds")
    display_time_unit = benchmark::kMillisecond;
  else if (val2 == "us" || val2 == "microsecond" || val2 == "microseconds")
    display_time_unit = benchmark::kMicrosecond;
  else if (val2 == "ns" || val2 == "nanosecond" || val2 == "nanoseconds")
    display_time_unit = benchmark::kNanosecond;
  else if (val2 == "s" || val2 == "second" || val2 == "seconds")
    display_time_unit = benchmark::kSecond;
  else
    return false;
  return true;
}
DEFINE_string(time_unit, "ms", "Unit of time");
DEFINE_validator(time_unit, &validate_unit_option);

int rand_range(int lower, int upper) {
  long randval = rand();
  return (randval % (upper - lower + 1)) + lower;
}

template <class T>
T rand_select(std::vector<T> &choices) {
  int idx = rand_range(0, choices.size() - 1);
  return choices[idx];
}

std::string gen_rand_string(int minlen, int maxlen) {
  static std::string alphabet("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
  int len = rand_range(minlen, maxlen);
  std::string result;
  for (int i = 0; i < len; ++i) {
    result += alphabet[rand_range(0, alphabet.size() - 1)];
  }
  return result;
}

size_t fill_directory(Directory *d) {
  int nchildren = rand_range(FLAGS_dirent_min, FLAGS_dirent_max);
  size_t total_size = 0;;
  for (int i = 0; i < nchildren; ++i) {
    std::string name = gen_rand_string(1, NAME_MAX);
    auto it = d->_children().find(name);
    if (it != d->_children().end()) {
      i--;
      continue;
    }
    d->_children()[name] = 0;
    //d->_children().insert(std::make_pair(std::string(name), 0));
    total_size += name.capacity() + sizeof(std::string) + sizeof(fuse_ino_t) +
      sizeof(std::_Rb_tree_node_base);
  }
  return total_size;
}

Inode *create_inode() {
  double rn = (double)rand() / RAND_MAX;
  Inode *res;
  size_t total_size = 0; 

  Directory *dir = new Directory(); 
  total_size += fill_directory(dir) + sizeof(Directory);
  res = dynamic_cast<Inode *>(dir);
  res->_attrs().st_mode = S_IFDIR;
  res->_attrs().st_size = total_size;

  return res;
}

size_t generate_inodes(std::vector<Inode *> &table) {
  size_t total_size = 0;
  for (int i = 0; i < FLAGS_inodes; ++i) {
    Inode *inode = create_inode();
    table.push_back(inode);
    total_size += sizeof(Inode *) + inode->Size();
  }
  return total_size;
}

void destroy_inodes(std::vector<Inode *> &table) {
  for (Inode *inode : table) {
    delete inode;
  }
  table.clear();
}

static std::vector<Inode *> source;
static std::vector<File> files;
static size_t total_size;
static void *data;

class CopyBenchTool {
public:
  static constexpr double eps = 1e-6;
  void SetUp() {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("Generating inode table...");
    total_size = generate_inodes(source);
    printf("%zu bytes (%.2f MB)\n", total_size, 1.0 * total_size / 1024 / 1024);  
  }

  void TearDown() {
    destroy_inodes(source);
    free(data);
  }
};

void update_directories(std::vector<Inode *> src) {
  int nqueries = 500;

  for(int i = 0; i < src.size(); i++) {
    Inode* curr = src[i];
    Directory *dir = dynamic_cast<Directory *>(curr);
    for (int i = 0; i < nqueries; ++i) {
      std::string name = gen_rand_string(1, NAME_MAX);
      dir->UpdateChild(name, 0);
    }
  }
}

void remove_from_directories(std::vector<Inode *> src) {
  int nqueries = 500;

  for(int i = 0; i < src.size(); i++) {
    Inode* curr = src[i];
    Directory *dir = dynamic_cast<Directory *>(curr);
    for (int i = 0; i < nqueries; ++i) {
      std::string name = gen_rand_string(1, NAME_MAX);
      dir->RemoveChild(name);
    }
  }
}

void query_directories(std::vector<Inode *> src) {
  int nqueries = 500;
  
  for(int i = 0; i < src.size(); i++) {
    Inode* curr = src[i];
    Directory *dir = dynamic_cast<Directory *>(curr);
    for (int i = 0; i < nqueries; ++i) {
      std::string name = gen_rand_string(1, NAME_MAX);
      dir->ChildInodeNumberWithName(name);
    }
  }
}

static void BM_RemoveFromDirectories(benchmark::State &state) {
  for (auto _ : state) {
    remove_from_directories(source);
  }
}

static void BM_UpdateDirectories(benchmark::State &state) {
  for (auto _ : state) {
    update_directories(source);
  }
}

static void BM_QueryDirectories(benchmark::State &state) {
  for (auto _ : state) {
    query_directories(source);
  }
}

int main(int argc, char **argv) {
  ::benchmark::Initialize(&argc, argv);
  gflags::ParseCommandLineFlags(&argc, &argv, false);
  // The purpose of this is to "use" a function in libprofilee so that the
  // compiler will always link the library.
  ProfilerEnable();
  CopyBenchTool copybench;
  copybench.SetUp();
  BENCHMARK(BM_UpdateDirectories)->Unit(display_time_unit);
  BENCHMARK(BM_QueryDirectories)->Unit(display_time_unit);
  BENCHMARK(BM_RemoveFromDirectories)->Unit(display_time_unit);
  ::benchmark::RunSpecifiedBenchmarks();
  copybench.TearDown();
}
