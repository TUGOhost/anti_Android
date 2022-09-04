#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/personality.h>
#include <sys/time.h>
#include <unistd.h>

#if defined(__i386__)
#define __LIBC_HIDDEN__ __attribute__((visibility("hidden")))

__attribute__((__naked__)) static void __libc_int0x80() {
  __asm__ volatile("int $0x80; ret");
}

__LIBC_HIDDEN__ void* __libc_sysinfo = (void*)(__libc_int0x80);

// TODO: lose this function and just access __libc_sysinfo directly.
__LIBC_HIDDEN__ void* __kernel_syscall() {
  return __libc_sysinfo;
}

#endif

