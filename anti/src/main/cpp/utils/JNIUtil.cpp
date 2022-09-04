//
// Created by tg on 2022/9/4.
//

#include "JNIUtil.h"
#include "../JNIHelper/JNIHelper.hpp"

jobject JNIUtil::getGlobalContext(JNIEnv *env) {
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID( activityThread,
                                                                "currentActivityThread",
                                                                "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);

    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication",
                                                   "()Landroid/app/Application;");
    jobject context = env->CallObjectMethod(at, getApplication);
    return context;
}

jobject JNIUtil::callMethodByName(JNIEnv *env, jobject context, char *function_name, char *sig) {
    if (nullptr == context) {
        return nullptr;
    }

    jmethodID jmethodId;
    jobject ret;

    jclass clazz = env->GetObjectClass(context);

    jmethodId = env->GetMethodID(clazz, function_name, sig);

    if (jmethodId == nullptr) {
        return nullptr;
    }
    ret = env->CallObjectMethod(context, jmethodId);
    if (ret != nullptr) {
        return ret;
    } else {
        return nullptr;
    }
}

jstring JNIUtil::getObjectField(JNIEnv *env, jobject context, char *field_name, char *sig) {
    jfieldID fid;
    jobject i;

    jclass cls = env->GetObjectClass(context);

    fid = env->GetFieldID(cls, field_name, sig);
    if (fid == nullptr) {
        return 0;
    }

    i = env->GetObjectField(context, fid);

    return static_cast<jstring>(i);
}

void JNIUtil::delete_ref(JNIEnv *env, jobject context) {
    jobjectRefType ref_type =
            context != nullptr ? env->GetObjectRefType(context) : JNIInvalidRefType;

    if (ref_type == JNIGlobalRefType) {
        env->DeleteGlobalRef(context);
    } else if (ref_type == JNILocalRefType) {
        env->DeleteLocalRef(context);
    } else if (ref_type == JNIWeakGlobalRefType) {
        env->DeleteWeakGlobalRef(context);
    }
}

std::string  JNIUtil::get_data_dir(JNIEnv *env) {
    jobject jcontext = getGlobalContext(env);
    jobject japplication_info = callMethodByName(env, jcontext, "getApplicationInfo",
                                                 "()Landroid/content/pm/ApplicationInfo;");

    jstring jdata_dir = getObjectField(env, japplication_info, "dataDir",
                                       "Ljava/lang/String;");

    std::string path = jh::jstringToStdString(jdata_dir);

    delete_ref(env, japplication_info);
    delete_ref(env, jdata_dir);
    return path;
}
