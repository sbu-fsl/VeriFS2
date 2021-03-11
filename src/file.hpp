/** @file file.hpp
 *  @copyright 2016 Peter Watkins. All rights reserved.
 */

#ifndef file_hpp
#define file_hpp

class File : public Inode {
private:
    void *m_buf;

    void copy_from_other(const File &f) {
      size_t bufsize = f.m_fuseEntryParam.attr.st_size; 
      m_buf = malloc(bufsize);
      memcpy(m_buf, f.m_buf, bufsize);
    }
    
public:
    File() :
    m_buf(NULL) {}

#ifdef VERIFS2_COPY_BENCH
    File(void *buf, size_t bufsize) : m_buf(buf) {
      m_fuseEntryParam.attr.st_size = bufsize;
    }
#endif

    File(const File &f) : Inode(f) {
      copy_from_other(f);
    }

    File& operator=(const File &f) {
      if (&f != this) {
        Inode::operator=(f);
        copy_from_other(f);
      }
      return *this;
    }
    
    ~File();
    
    int WriteAndReply(fuse_req_t req, const char *buf, size_t size, off_t off);
    int ReadAndReply(fuse_req_t req, size_t size, off_t off);
    int FileTruncate(size_t newSize);

#ifdef VERIFS2_COPY_BENCH
    /* Only for use for benchmarking purpose! */
    void _set_buffer(void *buf) {
      m_buf = buf;
    }

    void *_get_buffer(void) {
      return m_buf;
    }
#endif
    
//    size_t Size();
};

#endif /* file_hpp */
