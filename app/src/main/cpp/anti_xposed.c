#include <jni.h>

JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiXposed(JNIEnv *env, jclass clazz) {
    jstring jresult = ( *env)->NewStringUTF(env, "security");
    return jresult;
}
