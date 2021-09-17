//
// Created by tg on 2021/6/30.
//

#include <jni.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "jni_utils.h"
#include "logging.h"

bool detecte_parent_dir(JNIEnv *);

JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiVirtualApp(JNIEnv *env, jclass clazz) {
    jstring jresult = NULL;
    if (detecte_parent_dir(env)) {
        jresult = ( *env)->NewStringUTF(env, "detected va");
    } else {
        jresult = (*env)->NewStringUTF(env, "security");
    }
    return jresult;
  // TODO
}

// 检测所有的父目录是否可读，因为VA在创建一个app的时候是在自己的app空间下创建的
bool detecte_parent_dir(JNIEnv *env) {

    jobject jcontext = getGlobalContext(env);
    jobject japplication_info = callMethodByName(env, jcontext, "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
    jstring jdata_dir = (jstring)getObjectField(env, japplication_info, "dataDir", "()Ljava/lang/String;");
    char * path = jstring_to_char(env, jdata_dir);
    delete_ref(env, japplication_info);
    delete_ref(env, jdata_dir);
    bool result = false;

    for (int i = strlen(path); i > 0 ; i --) {
        if (path[i] != '/') {
            path[i] = '\0';
        } else {
            if (access(path, R_OK) == 0) {
                result = true;
                break;
            }
        }
    }

    return result;
}
