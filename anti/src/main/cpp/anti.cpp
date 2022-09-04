#include <jni.h>
#include <string>
#include "check/anti_frida.h"
#include "check/anti_dual_app.h"
#include "JNIHelper/JNIHelper.hpp"

#define JNI_CLASS_NAME "com/tg/android/anti/NativeLib"

static JavaVM* jvm;

static jstring anti_frida(JNIEnv *env, jclass clazz) {
    AntiFrida antiFrida;
    antiFrida.check();

    return jh::createJString("anti frida...");
}

static jstring anti_xposed(JNIEnv *env, jclass clazz) {

    return jh::createJString("security");
}

static jstring anti_root(JNIEnv *env, jclass clazz) {
    return jh::createJString("security");
}

static jstring anti_debug(JNIEnv *env, jclass clazz) {
    return jh::createJString("security");
}

static jstring anti_mem_dump(JNIEnv *env, jclass clazz) {
    return jh::createJString("security");
}

static jstring anti_emulator(JNIEnv *env, jclass clazz) {
    return jh::createJString("security");
}

static jstring anti_dual_app(JNIEnv *env, jclass clazz) {
    AntiDualApp antiDualApp;

    return jh::createJString(antiDualApp.check());
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    (void)reserved;

    if (__predict_false(nullptr == vm)) return JNI_ERR;

    JNIEnv *env;
    if (__predict_false(JNI_OK != vm->GetEnv((void **)&env, JNI_VERSION_1_6))) return JNI_ERR;
    if (__predict_false(nullptr == env)) return JNI_ERR;

    jclass cls;
    if (__predict_false(nullptr == (cls = env->FindClass(JNI_CLASS_NAME)))) return JNI_ERR;

    JNINativeMethod  m[] =
            {
            {"AntiFrida", "()Ljava/lang/String;", (void *) anti_frida},
            {"AntiXposed", "()Ljava/lang/String;", (void *) anti_xposed},
            {"AntiRoot", "()Ljava/lang/String;", (void *) anti_root},
            {"AntiDebug", "()Ljava/lang/String;", (void *) anti_debug},
            {"AntiMemDump", "()Ljava/lang/String;", (void *) anti_mem_dump},
            {"AntiEmulator", "()Ljava/lang/String;", (void *) anti_emulator},
            {"AntiDualApp", "()Ljava/lang/String;", (void *) anti_dual_app},
    };

    if (__predict_false(0 != env->RegisterNatives(cls, m, sizeof(m) / sizeof(m[0])))) return JNI_ERR;

    jvm = vm;

    return JNI_VERSION_1_6;
}


namespace JNI
{
    JavaVM* getVM()
    {
        return jvm;
    }
}