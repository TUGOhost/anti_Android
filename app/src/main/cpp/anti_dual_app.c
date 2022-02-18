//
// Created by tg on 2021/6/30.
//

#include <jni.h>
#include <stdarg.h>
#include <malloc.h>
#include <fcntl.h>
#include <string.h>

#include "jni_utils.h"
#include "logging.h"
#include "mini_io/_mini_io.h"
#include "utils.h"

bool check_dual_app(JNIEnv *);

JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiDualApp(JNIEnv *env, jclass clazz) {
    jstring jresult = (*env)->NewStringUTF(env, "security");
    bool result = check_dual_app(env);
    if (result) {
        jresult = (*env)->NewStringUTF(env, "detected dual app");
    }
    return jresult;
//TODO
}

// https://bbs.pediy.com/thread-255212.htm
bool check_dual_app(JNIEnv *env) {
    char *self_fd = "/proc/self/fd/";

    bool result = false;
    char *simple_name = "wtf_jack";

    char *data_dir_path = get_data_dir(env);

    char *separator = "/";

    char *test_path;
    size_t length = strcat2(&test_path, data_dir_path, separator, simple_name);

    if (length < 0) {
        return result;
    }

    //if (test_path == NULL) return result;

    int flag = O_RDWR | O_CREAT | O_TRUNC;

    int fd = __openat(AT_FDCWD, test_path, flag, 0644);

    if (fd == -1) {
        result = true;
        return result;
    }


    char fd_Path[80];
    sprintf(fd_Path, "%s%d", self_fd, fd);

    char buff[1024];

    // 使用_readlinkat返回真实的地址。
    ssize_t len = _readlinkat(AT_FDCWD, fd_Path, buff, sizeof(buff));

    if (len < 0) {
        result = true;
        return result;
    }

    buff[len] = '\0';

    int count_file = 0;
    // todo
    for (int i = strlen(buff); i > 0; i--) {
        if (buff[i] != '/') {
            buff[i] = '\0';
        } else {
            if (count_file == 0) {
                count_file ++;
            } else {
                if (access(buff, R_OK) == 0) {

                    result = true;
                    break;
                }
            }
        }
    }

    if (test_path != NULL) free(test_path);

    return result;
}

bool check_android_for_work(JNIEnv *env) {
    char *data_path = "/data/data/";

    char *data_dir_path = get_data_dir(env);

    bool result = false;

    if (strstr(data_dir_path, data_path) != NULL) {
        // android 4.x 5.0
        int count_file = 0;
        for (int i = strlen(data_dir_path); i > 0; i--) {
            if (data_dir_path[i] != '/') {
                data_dir_path[i] = '\0';
            } else {
                if (count_file == 0) {
                    count_file ++;
                } else {
                    if (access(data_dir_path, R_OK) == 0) {

                        result = true;
                        break;
                    }
                }
            }
        }
    } else {
        // android 6.0 +
        if (data_dir_path[11] != '0') {
            result = true;
        }
    }

    return result;
}
