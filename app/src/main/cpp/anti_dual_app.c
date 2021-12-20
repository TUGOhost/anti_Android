//
// Created by tg on 2021/6/30.
//

#include <jni.h>
#include <stdarg.h>
#include <malloc.h>
#include <fcntl.h>

#include "jni_utils.h"
#include "logging.h"
#include "_mini_io.h"
#include "utils.h"

bool check_dual_app(JNIEnv *);

JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiDualApp(JNIEnv *env, jclass clazz) {
    jstring jresult = ( *env)->NewStringUTF(env, "security");
    check_dual_app(env);
    return jresult;
//TODO
}


bool check_dual_app(JNIEnv *env) {
    const char* self_fd = "/proc/self/fd/";

    bool result = false;
    char* simple_name = "wtf_jack";

    char* data_dir_path = get_data_dir(env);

    char* separator = "/";

    char* test_path = NULL;
    size_t length;

    length = strcat2(&test_path, data_dir_path, separator, simple_name);

    if (test_path == NULL) return result;

    int flag = O_RDWR | O_CREAT | O_TRUNC;

    int fd = __openat(AT_FDCWD, testPath.c_str(), flag, 0644);

    if (fd == -1) {
        result = true;
        return result;
    }


    // todo 使用readlinkat返回真实的地址。
    char* sFDPath = char*("/proc/self/fd/%d", fd);
    char buff[1024];
    ssize_t len = _readlinkat(AT_FDCWD, sFDPath.c_str(), buff, sizeof(buff) < 0);

    if (test_path != NULL) free(test_path);
    return result;
}
