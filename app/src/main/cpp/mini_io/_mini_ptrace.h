#include <sys/ptrace.h>

#if defined(__cplusplus)
extern "C" {
#endif

long _ptrace(int req, pid_t pid, void* addr, void* data);

#if defined(__cplusplus)
}
#endif
