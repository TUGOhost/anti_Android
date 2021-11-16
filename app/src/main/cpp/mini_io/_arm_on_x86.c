#include "_mini_io.h"
#include <fcntl.h>
#include <elf.h>
#include <string.h>

static bool arm_on_x86 = false;
static bool checked = false;
bool _arm_on_x86() {
#if defined(__aarch64__) || defined(__arm__)
    if(checked) {
       return  arm_on_x86;
    }
    int fd = open("/proc/self/exe", O_RDONLY);
    Elf32_Ehdr hdr;
    memset(&hdr, 0 , sizeof(hdr));
    if(read(fd, &hdr, sizeof(hdr)) < 0) {
        arm_on_x86 = false;
    } else {
        arm_on_x86 = hdr.e_machine == EM_386 ||  hdr.e_machine == EM_X86_64;
    }
    close(fd);
    checked = true;
    return arm_on_x86;
#elif defined(__i386__) || defined(__x86_64__)
    return false;
#endif

}
