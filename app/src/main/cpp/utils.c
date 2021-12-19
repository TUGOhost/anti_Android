//
// Created by tg on 2021/8/5.
//
// utils.c
#include <jni.h>
#include <string.h>
#include <malloc.h>

void ToastUtil(JNIEnv *env, jobject object, char *message){
    jstring toast_msg = (jstring)(*env)->NewStringUTF(env,message);
    jclass myclass = (*env)->FindClass(env,"com/test/FirstApplication");
    jmethodID mid = (*env)->GetMethodID(env,myclass,"showToast","(Ljava/lang/String;)V");
    (*env)->CallVoidMethod(env,object,mid,toast_msg);
}

static size_t strcat2(char **dst_out, ...)
{
    size_t len = 0, len_sub;
    va_list argp;
    char *src;
    char *dst = NULL, *dst_p;

    *dst_out = NULL;

    va_start(argp, dst_out);
    for (;;)
    {
        if ((src = va_arg(argp, char *)) == NULL) break;
        len += strlen(src);
    }
    va_end(argp);

    if (len == 0) return 0;

    dst = (char *)malloc(sizeof(char) * (len + 1));
    if (dst == NULL) return -1;
    dst_p = dst;

    va_start(argp, dst_out);
    for (;;)
    {
        if ((src = va_arg(argp, char *)) == NULL) break;
        len_sub = strlen(src);
        memcpy(dst_p, src, len_sub);
        dst_p += len_sub;
    }
    va_end(argp);
    *dst_p = '\0';

    *dst_out = dst;

    return len;
}