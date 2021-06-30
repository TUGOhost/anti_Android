//
// Created by tg on 2021/6/29.
//

#include <jni.h>
#include <unistd.h>
#include <stdio.h>
#define  MAX_LINE 1024

// anti frida with read maps
extern "C" bool readMaps() {
    char* buf;
    FILE *fp;

    pid_t pid = getpid();
    char maps[127] = {0};
    snprintf(maps, sizeof(maps), "/proc/%d/maps/", pid);
    if ((fp = fopen(maps,"r")) == NULL) {
        perror("failed to read maps");
        return false;
    }
    while (fgets(buf, MAX_LINE, fp) != NULL) {

        if (strstr(buf, "frida") != NULL) {
            return true;
        }
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_AntiFrida_App(JNIEnv *env, jclass clazz) {
    if (readMaps()) {
        env->NewStringUTF("has Frida");
    } else {
        env->NewStringUTF("security");
    }
}

