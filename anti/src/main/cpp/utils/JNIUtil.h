//
// Created by tg on 2022/9/4.
//

#ifndef ANTI_ANDROID_JNIUTIL_H
#define ANTI_ANDROID_JNIUTIL_H

#include "jni.h"
#include <string>

class JNIUtil {
public:
    static jobject getGlobalContext(JNIEnv *env);

    static jobject callMethodByName(JNIEnv *env, jobject context, std::string function_name, std::string sig);

    static jstring getObjectField(JNIEnv *env, jobject context, std::string field_name, std::string sig);

    static void delete_ref(JNIEnv *env, jobject context);

    static std::string get_data_dir(JNIEnv *env);
};


#endif //ANTI_ANDROID_JNIUTIL_H
