//
// Created by tg on 2021/9/17.
//

#include "jni_utils.h"

jobject getGlobalContext(JNIEnv *env) {

    jclass activityThread = (*env)->FindClass(env, "android/app/ActivityThread");
    jmethodID currentActivityThread = (*env)->GetStaticMethodID(env, activityThread,
                                                                "currentActivityThread",
                                                                "()Landroid/app/ActivityThread;");
    jobject at = (*env)->CallStaticObjectMethod(env, activityThread, currentActivityThread);

    jmethodID getApplication = (*env)->GetMethodID(env, activityThread, "getApplication",
                                                   "()Landroid/app/Application;");
    jobject context = (*env)->CallObjectMethod(env, at, getApplication);
    return context;
}

jobject callMethodByName(JNIEnv *env, jobject context, char *function_name, char *sig) {
    if (NULL == context) {
        return NULL;
    }

    jmethodID jmethodId;
    jobject ret;

    jclass clazz = (*env)->GetObjectClass(env, context);

    jmethodId = (*env)->GetMethodID(env, clazz, function_name, sig);

    if (jmethodId == NULL) {
        return NULL;
    }
    ret = (*env)->CallObjectMethod(env, context, jmethodId);
    if (ret != NULL) {
        return ret;
    } else {
        return NULL;
    }
}

jstring getObjectField(JNIEnv *env, jobject context, char *field_name, char *sig) {
    jfieldID fid;
    jobject i;

    jclass cls = (*env)->GetObjectClass(env, context);

    fid = (*env)->GetFieldID(env, cls, field_name, sig);
    if (fid == NULL) {
        return 0;
    }

    i = (*env)->GetObjectField(env, context, fid);

    return i;
}

char *jstring_to_char(JNIEnv *env, jstring string) {
    if (NULL == string) {
        return "";
    }
    const char *nativeString = (*env)->GetStringUTFChars(env, string, 0);
    (*env)->ReleaseStringUTFChars(env, string, nativeString);
    return nativeString;
}

void delete_ref(JNIEnv *env, jobject context) {
    jobjectRefType ref_type =
            context != NULL ? (*env)->GetObjectRefType(env, context) : JNIInvalidRefType;

    if (ref_type == JNIGlobalRefType) {
        (*env)->DeleteGlobalRef(env, context);
    } else if (ref_type == JNILocalRefType) {
        (*env)->DeleteLocalRef(env, context);
    } else if (ref_type == JNIWeakGlobalRefType) {
        (*env)->DeleteWeakGlobalRef(env, context);
    }
}

char* get_data_dir(JNIEnv *env) {
    jobject jcontext = getGlobalContext(env);
    jobject japplication_info = callMethodByName(env, jcontext, "getApplicationInfo",
                                                 "()Landroid/content/pm/ApplicationInfo;");
    //fixme dat_dir is NULL
    jstring jdata_dir = getObjectField(env, japplication_info, "dataDir",
                                       "Ljava/lang/String;");

    char *path = jstring_to_char(env, jdata_dir);

    delete_ref(env, japplication_info);
    delete_ref(env, jdata_dir);
    return path;
}