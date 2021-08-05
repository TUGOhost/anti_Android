//
// Created by tg on 2021/6/30.
//

#include <jni.h>

JNIEXPORT jstring  JNICALL
Java_com_tg_anti_MainActivity_AntiDebug(JNIEnv *env, jclass clazz) {
    //jstring jresult = nullptr;
    jstring jresult = ( *env)->NewStringUTF(env, "security");
    return jresult;
}
