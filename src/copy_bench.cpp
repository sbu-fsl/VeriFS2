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

DEFINE_int32(inodes, 100000, "Number of inodes to generate");
DEFINE_int32(fsize_min, 0, "Minimum size of regular file (in bytes)");
DEFINE_int32(fsize_max, 65536, "Maximum size of regular file (in bytes)");
DEFINE_int32(dirent_min, 2, "Minimum number of children in each directory");
DEFINE_int32(dirent_max, 10000, "Max number of children in each directory");
DEFINE_double(file_ratio, 0.75, "Ratio of regular files");
DEFINE_double(dir_ratio, 0.15, "Ratio of directories");
DEFINE_double(symlink_ratio, 0.09, "Ratio of symlinks");
DEFINE_double(special_ratio, 0.01, "Ratio of special files");

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

void fill_buf_with_rand(void *buf, size_t size) {
  int *ptr = (int *)buf;
  for (size_t i = 0; i < size; i += sizeof(int), ++ptr) {
    int r = rand();
    if (size - i >= sizeof(int))
      *ptr = r;
    else
      memcpy(ptr, &r, size - i);
  }
}

size_t fill_file(File *f) {
  int datasize = rand_range(FLAGS_fsize_min, FLAGS_fsize_max);
  char *data = new char[datasize];
  fill_buf_with_rand(data, datasize);
  f->_set_buffer(data);
  return datasize;
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
    d->_children().insert({name, 0});
    total_size += name.capacity() + sizeof(std::string) + sizeof(fuse_ino_t) +
      sizeof(std::_Rb_tree_node_base);
  }
  return total_size;
}

Inode *create_inode() {
  double rn = (double)rand() / RAND_MAX;
  Inode *res;
  size_t total_size = 0; 

  // double file_threshold = 0;
  double dir_threshold = FLAGS_file_ratio;
  double symlink_threshold = FLAGS_file_ratio + FLAGS_dir_ratio;
  double special_threshold = FLAGS_file_ratio + FLAGS_dir_ratio +
    FLAGS_symlink_ratio;

  if (rn < dir_threshold) {
    File *file = new File();
    total_size += fill_file(file) + sizeof(File);
    res = dynamic_cast<Inode *>(file);
    res->_attrs().st_mode = S_IFREG;
  } else if (rn >= dir_threshold && rn < symlink_threshold) {
    Directory *dir = new Directory(); 
    total_size += fill_directory(dir) + sizeof(Directory);
    res = dynamic_cast<Inode *>(dir);
    res->_attrs().st_mode = S_IFDIR;
  } else if (rn >= symlink_threshold && rn < special_threshold) {
    std::string linkpath = gen_rand_string(1, PATH_MAX);
    SymLink *sym = new SymLink(linkpath);
    total_size += sizeof(SymLink) + linkpath.capacity();
    res = dynamic_cast<Inode *>(sym);
    res->_attrs().st_mode = S_IFLNK;
  } else if (rn >= special_threshold) {
    std::vector<SpecialInodeTypes> choices({
        SPECIAL_INODE_TYPE_NO_BLOCK, SPECIAL_INODE_CHAR_DEV,
        SPECIAL_INODE_BLOCK_DEV, SPECIAL_INODE_FIFO, SPECIAL_INODE_SOCK});
    enum SpecialInodeTypes itype = rand_select(choices);
    SpecialInode *sp = new SpecialInode(itype);
    total_size += sizeof(SpecialInode);
    res = dynamic_cast<Inode *>(sp);
    mode_t mode;
    switch(itype) {
      case SPECIAL_INODE_CHAR_DEV:
        mode = S_IFCHR;
        break;

      case SPECIAL_INODE_BLOCK_DEV:
        mode = S_IFBLK;
        break;

      case SPECIAL_INODE_FIFO:
        mode = S_IFIFO;
        break;

      case SPECIAL_INODE_SOCK:
        mode = S_IFSOCK;
        break;

      default:
        mode = 0;
        break;
    }
    res->_attrs().st_mode = mode;
    res->_attrs().st_dev = 0;
  }
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

void generate_files(std::vector<File> &files, size_t amount) {
  /* Determine size of the last element */
  ssize_t remaining = amount;
  while (remaining > 0) {
    size_t data_size = rand_range(FLAGS_fsize_min, FLAGS_fsize_max);
    void *data = malloc(data_size);
    assert(data != NULL);
    fill_buf_with_rand(data, data_size);
    files.emplace_back(data, data_size);
    remaining -= (data_size + sizeof(File));
  }
}

void copy_files(std::vector<File> &dest, const std::vector<File> &src) {
  for (const File &f : src) {
    dest.emplace_back(f);
  }
}

void copy_inodes(std::vector<Inode *> &dest, const std::vector<Inode *> &src) {
  for (size_t i = 0; i < src.size(); ++i) {
    Inode *src_i = src[i];
    if (S_ISREG(src_i->_attrs().st_mode)) {
      File *src_f = dynamic_cast<File *>(src_i);
      File *dest_f = new File(*src_f);
      dest.push_back(dynamic_cast<Inode *>(dest_f));
    } else if (S_ISDIR(src_i->_attrs().st_mode)) {
      Directory *src_d = dynamic_cast<Directory *>(src_i);
      Directory *dst_d = new Directory(*src_d);
      dest.push_back(dynamic_cast<Inode *>(dst_d));
    } else if (S_ISLNK(src_i->_attrs().st_mode)) {
      SymLink *src_l = dynamic_cast<SymLink *>(src_i);
      SymLink *dst_l = new SymLink(*src_l);
      dest.push_back(dynamic_cast<Inode *>(dst_l));
    } else {
      SpecialInode *src_sp = dynamic_cast<SpecialInode *>(src_i);
      SpecialInode *dst_sp = new SpecialInode(*src_sp);
      dest.push_back(dynamic_cast<Inode *>(dst_sp));
    }
  }
}

static std::vector<Inode *> source;
static std::vector<File> files;
static size_t total_size;
static void *data;

class CopyBenchTool {
public:
  static constexpr double eps = 1e-6;
  void SetUp() {
    double total_ratio = FLAGS_file_ratio + FLAGS_dir_ratio +
      FLAGS_symlink_ratio + FLAGS_special_ratio;
    if (std::abs(1.0 - total_ratio) > eps) {
      printf("file_ratio + dir_ratio + symlink_ratio + special_ratio "
             "must be equal to 1.0.\n");
      exit(1);
    }

    setvbuf(stdout, NULL, _IONBF, 0);
    printf("Generating inode table...");
    total_size = generate_inodes(source);
    printf("%zu bytes (%.2f MB)\n", total_size, 1.0 * total_size / 1024 / 1024);
    printf("Generating a list of files that have the same amount of data...");
    generate_files(files, total_size);
    printf("Done.\n");
    printf("Generating the same amount of contiguous data...");
    data = malloc(total_size);
    int *ptr = (int *)data;
    for (size_t i = 0; i < total_size; i += sizeof(int), ptr++) {
      *ptr = rand();
    }
    printf("Done.\n");
  }

  void TearDown() {
    destroy_inodes(source);
    free(data);
  }
};

static void BM_CopyInodeTable(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<Inode *> dest;
    copy_inodes(dest, source);

    state.PauseTiming();
    destroy_inodes(dest);
    state.ResumeTiming();
  }
}
BENCHMARK(BM_CopyInodeTable)->Unit(benchmark::kMillisecond);

static void BM_CopyFiles(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<File> dest;
    copy_files(dest, files);

    state.PauseTiming();
    dest.clear();
    state.ResumeTiming();
  }
}
BENCHMARK(BM_CopyFiles)->Unit(benchmark::kMillisecond);

static void BM_CopyData(benchmark::State &state) {
  for (auto _ : state) {
    void *copy_data = malloc(total_size);
    memcpy(copy_data, data, total_size);

    state.PauseTiming();
    free(copy_data);
    state.ResumeTiming();
  }
}
BENCHMARK(BM_CopyData)->Unit(benchmark::kMillisecond);

int main(int argc, char **argv) {
  ::benchmark::Initialize(&argc, argv);
  gflags::ParseCommandLineFlags(&argc, &argv, false);
  // if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
  CopyBenchTool copybench;
  copybench.SetUp();
  ::benchmark::RunSpecifiedBenchmarks();
  copybench.TearDown();
}
