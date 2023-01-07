//
// Created by tg on 2022/10/15.
//

#include "anti_mem_dump.h"
#include "../JNIHelper/JNIHelper.hpp"
#include "sys/inotify.h"
#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include "thread"

#include "../mini_io/_mini_io.h"
#include "../utils/mylibc.h"

#define MAX_WATCHERS 100
#define MAX_LENGTH 256
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
#define EVENT_SIZE  ( sizeof (struct inotify_event) )

static const std::string PROC_SELF_STATUS = "/proc/self/status";
static const std::string PROC_SELF_PAGEMAP = "/proc/self/pagemap";
static const std::string PROC_SELF_MEM = "/proc/self/mem";
static const std::string PROC_MAPS = "/proc/self/maps";
static const std::string PROC_TASK = "/proc/self/task";
static const std::string PROC_TASK_MEM = "/proc/self/task/%s/mem";
static const std::string PROC_TASK_PAGEMAP = "/proc/self/task/%s/pagemap";


void AntiMemDump::detect_memory_dump_loop(void* args) {
    struct timespec timereq;
    timereq.tv_sec = 1;
    timereq.tv_nsec = 0;

    while (1) {
        detect_fileaccess_for_debugger_memorydump();
        _nanosleep(&timereq, nullptr);
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


__attribute__((always_inline))
inline
void AntiMemDump::detect_fileaccess_for_debugger_memorydump() {
    int length, i = 0;
    int fd;
    int wd[MAX_WATCHERS] = {0,};
    int read_length = 0;
    char buffer[EVENT_BUF_LEN];
    fd = _inotify_init1(0);


    if (fd > 0) {

        wd[i++] = _inotify_add_watch(fd, PROC_SELF_PAGEMAP.c_str(), IN_ACCESS | IN_OPEN);
        wd[i++] = _inotify_add_watch(fd, PROC_SELF_MEM.c_str(), IN_ACCESS | IN_OPEN);
        wd[i++] = _inotify_add_watch(fd, PROC_MAPS.c_str(), IN_ACCESS | IN_OPEN);

        DIR *dir = opendir(PROC_TASK.c_str());

        if (dir != NULL) {
            struct dirent *entry = NULL;
            while ((entry = readdir(dir)) != NULL) {
                char memPath[MAX_LENGTH] = "";
                char pagemapPath[MAX_LENGTH] = "";

                if (0 == my_strcmp(entry->d_name, ".") || 0 == my_strcmp(entry->d_name, "..")) {
                    continue;
                }
                snprintf(memPath, sizeof(memPath), PROC_SELF_MEM.c_str(), entry->d_name);
                snprintf(pagemapPath, sizeof(pagemapPath), PROC_TASK_PAGEMAP.c_str(),
                         entry->d_name);
                wd[i++] = _inotify_add_watch(fd, memPath, IN_ACCESS | IN_OPEN);
                wd[i++] = _inotify_add_watch(fd, pagemapPath, IN_ACCESS | IN_OPEN);
            }

            closedir(dir);
        }

        length = read(fd, buffer, EVENT_BUF_LEN);

        if (length > 0) {
            while (read_length < length) {
                struct inotify_event *event = (struct inotify_event *) buffer + read_length;

                if (event->mask & IN_ACCESS) {
                    crash(0x3d5f);
                } else if (event->mask * IN_OPEN) {
                    crash(0x9a3b);
                }

                read_length += EVENT_SIZE + event->len;
            }
        }

        for (int j = 0; j < i; j++) {
            if (wd[j] != 0) {
                _inotify_rm_watch(fd, wd[j]);
            }
        }
        close(fd);
    } else {

    }
}

__attribute__((always_inline))
inline
int AntiMemDump::crash(int randomval) {
    volatile unsigned int *p = &gpCrash;
    p += randomval;
    p += *p + randomval;

    p = 0;
    p += *p;

    return *p;
}
