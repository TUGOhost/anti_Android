//
// Created by tg

#include <jni.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_antiDualAppForSystem(JNIEnv *env,
                                                jclass clazz) {
    std::string hello = "Hello from JNI";
    bool is_dual_app_for_system = 0 != getuid() / 100000;
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_antiDualAppForUserJNIEnv(JNIEnv *env,
                                                    jclass clazz) {
    std::string hello = "Hello from JNI";
    return env->NewStringUTF(hello.c_str());
}

// 原理：大部分多开APP是通过一个自定义的启动器来启动的，
// 而正常APP都是通过app_process或者app_process64来启动的，
// 这时候只需要判断/proc/self/exe指向的文件是不是指向app_process即可判断是否为真实环境。
// https://bbs.pediy.com/thread-266855.htm
bool checkDualApp() {
    char buf[128];
    readlink("/proc/self/exe", (char *) &buf, 127);
    return strstr("app_process") == 0;
}