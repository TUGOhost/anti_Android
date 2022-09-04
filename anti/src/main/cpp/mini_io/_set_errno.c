#include <errno.h>
#include "_mini_io.h"

int _errno;

long ___set_errno_internal(int n) {
  _errno = n;
  return -1;
}

