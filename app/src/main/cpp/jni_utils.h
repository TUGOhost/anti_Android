//
// Created by tg on 2021/9/17.
//

#ifndef ANTIPTRACE_JNI_UTILS_H
#define ANTIPTRACE_JNI_UTILS_H
#include <jni.h>

jobject getGlobalContext(JNIEnv *env);

jobject callMethodByName(JNIEnv *, jobject, char *,char *);

jstring getObjectField(JNIEnv *, jobject, char *,char *);

char * jstring_to_char(JNIEnv *, jstring);

void delete_ref(JNIEnv *, jobject);

char* get_data_dir(JNIEnv *);

#endif //ANTIPTRACE_JNI_UTILS_H