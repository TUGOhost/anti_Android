//
// Created by tg on 2021/6/30.
//

#include <jni.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "jni_utils.h"
#include "logging.h"

bool detecte_parent_dir(JNIEnv *);

JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiVirtualApp(JNIEnv *env, jclass clazz) {
    jstring jresult = (*env)->NewStringUTF(env, "security");
    if (detecte_parent_dir(env)) {
        jresult = (*env)->NewStringUTF(env, "detected va");
    }
    return jresult;
}

// 检测所有的父目录是否可读，因为VA在创建一个app的时候是在自己的app空间下创建的
// 思路来自：https://juejin.cn/post/6964673582924300296#heading-18
bool detecte_parent_dir(JNIEnv *env) {
    bool result = false;

    char* path = get_data_dir(env);

    if (path != NULL) {
        for (int i = strlen(path); i > 0; i--) {
            if (path[i] != '/') {
                path[i] = '\0';
            } else {
                if (access(path, R_OK) == 0) {
                    result = true;
                    break;
                }
            }
        }
    }


    free(path);
    path = NULL;

    return result;
}
