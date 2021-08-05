//
// Created by tg on 2021/6/30.
//

#include <jni.h>

JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiEmulator(JNIEnv *env, jclass clazz) {
    // TODO: implement AntiEmulator()
    jstring jresult = ( *env)->NewStringUTF(env, "security");
    return jresult;
}