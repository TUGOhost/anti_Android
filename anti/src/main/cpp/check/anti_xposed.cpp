#include "anti_xposed.h"
#include <jni.h>
#include "../xposeddetector/classloader.h"
#include "Utils.h"
#include "../JNIHelper/JNIHelper.hpp"
#include "../xposeddetector/xposed.h"

int xposed_status = NO_XPOSED;

int AntiXposed::get_xposed_status(JNIEnv *env, int sdk) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    classloader::checkClassLoader((C_JNIEnv *) env, sdk);

    xposed::checkCallStack((C_JNIEnv *) env);

    if (xposed_status == NO_XPOSED) {
        LOGE("xposed_status == NO_XPOSED");
        //return xposed_status;
    }

    xposed::checkCallStack((C_JNIEnv *) env);

    return xposed_status;
}