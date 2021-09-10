//
// Created by tg on 2021/8/5.
//
// utils.c
#include <jni.h>

void ToastUtil(JNIEnv *env, jobject object, char *message){
    jstring toast_msg = (jstring)(*env)->NewStringUTF(env,message);
    jclass myclass = (*env)->FindClass(env,"com/test/FirstApplication");
    jmethodID mid = (*env)->GetMethodID(env,myclass,"showToast","(Ljava/lang/String;)V");
    (*env)->CallVoidMethod(env,object,mid,toast_msg);
}