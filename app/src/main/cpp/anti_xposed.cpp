#include <jni.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define  POSED "posed"
#define MAX_LINE 256

// anti l_posed with read maps
extern "C" bool read_maps() {
    char *buf = nullptr;
    FILE *fp;

    pid_t pid = getpid();
    char maps[127] = {0};
    snprintf(maps, sizeof(maps), "/proc/%d/maps/", pid);
    if ((fp = fopen(maps, "r")) == nullptr) {
        return false;
    }
    while (fgets(buf, MAX_LINE, fp) != nullptr) {

        if (strstr(buf, POSED) != nullptr) {
            return true;
        }
    }
    return false;
}



extern "C" bool detect_l_posed() {
    if (read_maps()) {
        return true;
    }
    return false;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiXposed(JNIEnv *env, jclass clazz) {
    jstring jresult = nullptr;
    if (detect_l_posed()) {
        jresult = env->NewStringUTF("has xposed or l poxed");
    } else {
        jresult = env->NewStringUTF("security");
    }
    return jresult;
}
