/** @file util.hpp
 *  @copyright 2016 Peter Watkins. All rights reserved.
 */

#ifndef util_hpp
#define util_hpp

#include <sys/types.h>

#define INO_NOTFOUND    (fuse_ino_t) 0

// TODO: This looks like it was required before. Perhaps Sierra now includes it.
//#ifdef __MACH__
//#define CLOCK_REALTIME 0
//clock_gettime is not implemented on OSX.
//int clock_gettime(int /*clk_id*/, struct timespec* t);
//#endif

static inline size_t round_up(size_t value, size_t unit) {
        return (value + unit - 1) / unit * unit;
}

static inline size_t get_nblocks(size_t size, size_t blocksize) {
        return (size + blocksize - 1) / blocksize;
}

#endif /* util_hpp */
