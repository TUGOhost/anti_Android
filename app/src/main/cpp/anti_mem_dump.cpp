//
// Created by tg on 2021/6/30.
//

#include <jni.h>

extern "C"
JNIEXPORT jstring  JNICALL
Java_com_tg_anti_MainActivity_AntiMemDump(JNIEnv *env, jclass clazz) {
    jstring jresult = nullptr;
    jresult = env->NewStringUTF("security");
    return jresult;
}

