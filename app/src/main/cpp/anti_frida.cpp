#include <jni.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define  MAX_LINE 512
#define  MAX_LENGTH 256
#define  LIBC "libc.so"

static const char* FRIDA_THREAD_GUM_JS_LOOP = "gum_js_loop";
static const char* FRIDA_THREAD_GMAIN = "gmain";
static const char* FRIDA_NAMEDPIPE_LINJECTOR = "linjector";
static const char* PROC_MAPS = "/proc/self/maps";
static const char* PROC_STATUS = "/proc/self/task/%s/status";
static const char* PROC_FD = "/proc/self/fd";
static const char* PROC_TASK = "/proc/self/task";
static const char* TEMP_PATH = "/data/local/tmp";



// anti frida with read maps
extern "C" bool read_maps() {
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

// Frida Server Detection, reference:https://github.com/b-mueller/frida-detection-demo/blob/master/AntiFrida/app/src/main/cpp/native-lib.cpp
bool detect_frida_loop(void *) {
    struct sockaddr_in sa{};
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    inet_aton("127.0.0.1", &(sa.sin_addr));

    int sock;
    int fd;
    char map[MAX_LINE];
    char res[7];
    int num_found;
    int ret;
    int i;

    while (1) {
        for (int i = 0; i < 65535; ++i) {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            sa.sin_port = htons(i);

            if (connect(sock, (struct sockaddr*)&sa, sizeof sa) != -1) {
                memset(res, 0, 7);

                send(sock, "\x00", 1, NULL);
                send(sock, "AUTH\r\n", 6, NULL);

                usleep(100);

                if ((ret = recv(sock, res, 6, MSG_DONTWAIT)) != -1) {
                    if (strcmp(res, "REJECT") == 0) {
                        return true;
                    }
                }
            }

            close(sock);
        }
    }
    return false;
}


extern "C" bool detect_frida() {
    if (read_maps()) {
        return true;
    }
    return false;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_tg_anti_MainActivity_AntiFrida(JNIEnv *env, jclass clazz) {
    jstring jresult = nullptr;
    if (detect_frida()) {
        jresult = env->NewStringUTF("has Frida");
    } else {
        jresult = env->NewStringUTF("security");
    }
    pthread_t t;
    pthread_create(&t, NULL, reinterpret_cast<void *(*)(void *)>(detect_frida_loop), (void *)NULL);
    return jresult;
}
