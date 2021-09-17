#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/mman.h>

#include "logging.h"

static uint64_t frida_find_library_base(pid_t pid, const char *library_name,
                                   char **library_path);

static uint64_t _find_library_space_base(pid_t pid, uint64_t base,
                                         uint32_t page_size);

/**
 * 使用frida的实现原理漏洞， frida会使用目标的libc来加载和调用自身的so文件实现注入的效果。
 * 主要是向maps表中插入一个只读的libc即可。
 */
void anti_frida() {
    char *path = NULL;
    int page_size = getpagesize();
    pid_t pid = getpid();
    uint64_t start = frida_find_library_base(pid, "libc", &path);

    if (start != 0 && path != NULL && strlen(path) > 0) {

        uint64_t base = _find_library_space_base(pid, start, page_size);

        if (base != 0) {
            int fd = open(path, O_RDONLY);
            free(path);
            path = NULL;
            if (fd > 0) {
                void *p = mmap((void *) base, page_size, PROT_READ, MAP_PRIVATE, fd, 0);
                close(fd);

                if (p != MAP_FAILED) {
                    uint64_t start2 = frida_find_library_base(pid, "libc", NULL);
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
uint64_t frida_find_library_base(pid_t pid, const char *library_name, char **library_path) {
    uint64_t result = 0;
    char *maps_path;
    FILE *fp;
    const uint64_t line_size = 1024 + PATH_MAX;
    char *line, *path;

    if (library_path != NULL)
        *library_path = NULL;

    sprintf(maps_path, "/proc/%d/maps", pid);

    fp = fopen(maps_path, "r");

    free(maps_path);

    line = malloc(line_size);
    path = malloc(PATH_MAX);

    while (result == 0 && fgets(line, line_size, fp) != NULL) {
        uint64_t start;
        int n;

        n = sscanf(line, "%"
        PRIx64
        "-%*x %*s %*x %*s %*s %s", &start, path);
        if (n == 1)
            continue;
//g_assert (n == 2);

        if (path[0] == '[')
            continue;

        if (strcmp(path, library_name) == 0) {
            result = start;
            if (library_path != NULL)
                *library_path = strdup(path);
        }
#ifdef HAVE_ANDROID
            else if (g_str_has_prefix (path, "/system_root") && strcmp (path + strlen ("/system_root"), library_name) == 0)
                {
                  result = start;
                  if (library_path != NULL)
                    *library_path = g_strdup (path);
                }
#endif
        else {
            char *p = strrchr(path, '/');
            if (p != NULL) {
                p++;

                if (str_has_prefix(p, library_name) && strstr(p, ".so")) {
                    char next_char = p[strlen(library_name)];
                    if (next_char == '-' || next_char == '.') {
                        result = start;
                        if (library_path != NULL)
                            *library_path = strdup(path);
                    }
                }
            }
        }
    }

    free(path);
    free(line);

    fclose(fp);

    return result;
}

uint64_t _find_library_space_base(pid_t pid, uint64_t base,
                                  uint32_t page_size) {
    char maps_path[1000];
    FILE *fp;
    const size_t line_size = 1024 + 1024;
    char *line;

    sprintf(maps_path, "/proc/%d/maps", pid);
    fp = fopen(maps_path, "r");
    line = malloc(line_size);

    uint64_t last_end = 0;

    while (fgets(line, line_size, fp) != NULL) {
        uint64_t start;
        uint64_t end;
        int n;

        //     if (strstr(line, "libc")) {
        //       int x = strlen(line);
        //       line[x - 1] = 0;
        //       printf("> %s\n", line);
        //     }

        n = sscanf(line, "%"
        PRIx64
        "-%"
        PRIx64
        "", &start, &end);
        if (n != 2) {
            continue;
        }

        if (last_end == 0 && start == base) { // this is the first page
            last_end = start - page_size;
            break;
        }
        if (last_end == 0) { // always mmap after first entry
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

    fclose(fp);

    return last_end;
}


JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiFrida(JNIEnv *env, jclass clazz) {
    anti_frida();
    jstring jresult = ( *env)->NewStringUTF(env, "security");
    return jresult;
// TODO
}