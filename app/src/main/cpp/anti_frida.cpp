#include <jni.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define  MAX_LINE 512
#define  MAX_LENGTH 256
static const char* FRIDA_THREAD_GUM_JS_LOOP = "gum_js_loop";
static const char* FRIDA_THREAD_GMAIN = "gmain";
static const char* FRIDA_NAMEDPIPE_LINJECTOR = "linjector";
static const char* PROC_MAPS = "/proc/self/maps";
static const char* PROC_STATUS = "/proc/self/task/%s/status";
static const char* PROC_FD = "/proc/self/fd";
static const char* PROC_TASK = "/proc/self/task";
#define LIBC "libc.so"

// anti frida with read maps
extern "C" bool readMaps() {
    char *buf = nullptr;
    FILE* fp;

    pid_t pid = getpid();
    char maps[127] = {0};
    snprintf(maps, sizeof(maps), "/proc/%d/maps/", pid);
    if ((fp = fopen(maps,"r")) == nullptr) {
        return false;
    }
    while (fgets(buf, MAX_LINE, fp) != nullptr) {

        if (strstr(buf, "frida") != nullptr) {
            return true;
        }
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiFrida(JNIEnv *env, jclass clazz) {
    jstring jresult = nullptr;
    if (readMaps()) {
        jresult = env->NewStringUTF("has Frida");
    } else {
        jresult = env->NewStringUTF("security");
    }
    return jresult;
}
