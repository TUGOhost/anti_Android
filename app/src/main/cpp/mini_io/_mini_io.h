#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif



extern int _errno;
long ___set_errno_internal(int n);
int ___close(int);
int __openat(int, const char *, int, int);
int _open(const char *pathname, int flags, ...);
int _close(int fd);
ssize_t _write(int __fd, const void *__buf, size_t __count);
ssize_t _read(int fd, void *buf, size_t count);
off_t _lseek(int __fd, off_t __offset, int __whence);


bool _arm_on_x86();

#if defined(__cplusplus)
}
#endif
