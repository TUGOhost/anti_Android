#include "_mini_io.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

static inline int force_O_LARGEFILE(int flags) {
#if defined(__LP64__)
  return flags; // No need, and aarch64's strace gets confused.
#else
  return flags | O_LARGEFILE;
#endif
}

#ifndef O_TMPFILE
#define O_TMPFILE (020000000 | 00200000)
#endif

static inline bool needs_mode(int flags) {
  return ((flags & O_CREAT) == O_CREAT) || ((flags & O_TMPFILE) == O_TMPFILE);
}

int _open(const char *pathname, int flags, ...) {
  mode_t mode = 0;

  if (needs_mode(flags)) {
    va_list args;
    va_start(args, flags);
    mode = (mode_t)(va_arg(args, int));
    va_end(args);
  }

  return __openat(AT_FDCWD, pathname, force_O_LARGEFILE(flags), mode);
}

int _close(int fd) {
  // if(_arm_on_x86()){
  //		return close(fd);
  //	}
  int rc = ___close(fd);
  if (rc == -1 && _errno == EINTR) {
    return 0;
  }
  return rc;
}
