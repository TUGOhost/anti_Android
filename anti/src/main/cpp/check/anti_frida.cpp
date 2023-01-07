//
// Created by tg on 2022/9/3.
//

#include <inttypes.h>
#include <sys/mman.h>
#include <malloc.h>
#include <pthread.h>
#include <fcntl.h>

#include "anti_frida.h"
#include "../mini_io/_mini_io.h"


void AntiFrida::check() {
    char *libc_path = nullptr;
    int page_size = getpagesize();
    uint64_t start = frida_find_library_base("libc", &libc_path);

    if (start != 0 && nullptr != libc_path && strlen(libc_path) > 0) {
        uint64_t base = frida_find_library_space_base(start, page_size);

        if (base != 0) {
            int fd = _open(libc_path, O_RDONLY);

            if (fd > 0) {
                void *p = mmap((void *) base, page_size, PROT_READ, MAP_PRIVATE, fd, 0);
                close(fd);

                if (p != MAP_FAILED) {
                    uint64_t start2 = frida_find_library_base("libc", nullptr);
                } else {
                }
            }
        }
    }
}

bool str_has_prefix(const char *str1, const char *str2) {
    const char *x = strstr(str1, str2);
    return x == str1;
}

// copy from https://github.com/frida/frida-core/blob/836d254614d836e39d17418e3b864c8b5862bf9b/src/linux/frida-helper-backend-glue.c#L3014
uint64_t AntiFrida::frida_find_library_base(std::string library_name, char **library_path) {
    uint64_t result = 0;
    std::string maps_path = "/proc/self/maps";
    const std::size_t line_size = 1024 + PATH_MAX;
    char *line, *path;

    if (nullptr != library_path) {
        *library_path = nullptr;
    }

    FILE *fp = fopen(maps_path.c_str(), "r");

    line = static_cast<char *>(malloc(line_size));
    path = static_cast<char *>(malloc(PATH_MAX));

    while (result == 0 && fgets(line, line_size, fp) != nullptr) {
        uint64_t start;
        int n = 0;

        path[0] = 0;
        n = sscanf(line, "%"
                         PRIx64
                         "-%*x %*s %*x %*s %*s %s", &start, path);

        if (n != 2) {
            continue;
        }

        if (n == 1) {
            continue;
        }

        if (path[0] == '[') {
            continue;
        }

        if (strcmp(path, library_name.c_str()) == 0) {
            result = start;
            if (library_path != nullptr) {
                *library_path = strdup(path);
            }
        } else {
            char *p = strrchr(path, '/');
            if (p != nullptr) {
                p++;
                if (str_has_prefix(p, library_name.c_str()) && strstr(p, ".so")) {
                    char next_char = p[library_name.size()];
                    if (next_char == '-' || next_char == '.') {
                        result = start;
                        if (library_path != nullptr) {
                            *library_path = strdup(path);
                        }
                    }
                }
            }
        }
    }

    free(path);
    path = nullptr;
    free(line);
    line = nullptr;

    fclose(fp);
    fp = nullptr;

    return result;
}

uint64_t AntiFrida::frida_find_library_space_base(uint64_t base, uint32_t page_size) {
    std::string map_path = "/proc/self/maps";
    const std::size_t line_size = 1024 + 1024;
    char *line;

    FILE *fp = fopen(map_path.c_str(), "r");

    line = static_cast<char *>(malloc(line_size));

    uint64_t last_end = 0;

    while (fgets(line, line_size, fp) != nullptr) {
        uint64_t start, end;
        int n = 0;

        n = sscanf(line, "%"
                         PRIx64
                         "-%"
                         PRIx64
                         "", &start, &end);

        if (n != 2) {
            continue;
        }

        if (last_end == 0 && start == base) {
            last_end = start - page_size;
            continue;
        }

        if (last_end == 0) {
            last_end = end;
            continue;
        }

        if (start >= base) {
            last_end = 0;
            break;
        }

        if (start - page_size < last_end) {
            last_end = end;
            continue;
        }

        break;
    }

    free(line);
    line = nullptr;

    fclose(fp);
    fp = nullptr;

    return last_end;
}
