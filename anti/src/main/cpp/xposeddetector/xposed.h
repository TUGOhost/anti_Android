#ifndef ANTI_ANDROID_XPOSED_H
#define ANTI_ANDROID_XPOSED_H

#include <jni.h>
#include "anti_xposed.h"

extern int xposed_status;

class xposed {
public:
    static void doAntiXposed(C_JNIEnv *env, jobject object, intptr_t hash);

    static void checkCallStack(C_JNIEnv *env);

private:
    static jclass findLoadedClass(C_JNIEnv *env, jobject classLoader, const char *name);

    static jclass findXposedBridge(C_JNIEnv *env, jobject classLoader);

    static jclass findXposedHelper(C_JNIEnv *env, jobject classLoader);

    static bool disableXposedBridge(C_JNIEnv *env, jclass classXposedBridge);

    static jfieldID findMapField(C_JNIEnv *env, jclass classXposedBridge);

    static bool doClearHooksClass(C_JNIEnv *env, jclass classXposedBridge);

    static bool doClearHooksCommon(C_JNIEnv *env, jobject classLoader, const char *name);

    static bool clearHooks(C_JNIEnv *env, jobject classLoader);

};


#endif //ANTI_ANDROID_XPOSED_H
