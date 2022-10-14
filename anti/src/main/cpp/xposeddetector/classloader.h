#ifndef ANTI_ANDROID_CLASSLOADER_H
#define ANTI_ANDROID_CLASSLOADER_H

#include "jni.h"

class classloader {
public:
    static void checkClassLoader(C_JNIEnv *env, int sdk);

private:
    static void checkGlobalRef(C_JNIEnv *env, jclass clazz);

    static void checkWeakGlobalRef(C_JNIEnv *env, jclass clazz);
};


#endif //ANTI_ANDROID_CLASSLOADER_H
