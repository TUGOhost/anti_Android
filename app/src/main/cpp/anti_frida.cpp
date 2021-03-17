#include <jni.h>
#include <string>
#include <sys/ptrace.h>
#include <string>
#include <thread>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include "android/log.h"
#include <errno.h>
#include <elf.h>
#include <dlfcn.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <asm/unistd.h>
#include <stdio.h>
#include <android/log.h>
#include <sys/mman.h>
#include <stdbool.h>


#define LOG_TAG "tg"
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)
#define K 1024
#define WRITELEN (128 * K)
#define MAX (128 * K)


extern "C"
unsigned long getLibAddr(const char * lib)
{
    puts("Enter getLibAddr");
    unsigned long addr = 0;
    char lineBuf[256];

    snprintf(lineBuf, 256 - 1, "/proc/%d/maps/", getpid());
    FILE *fp = fopen(lineBuf, "r");
    if (fp == NULL)
    {
        perror("fopen failed");
        goto bail;
    }
    while (fgets(lineBuf , sizeof(lineBuf), fp)) {
        if (strstr(lineBuf, lib)) {
            char * temp = strtok(lineBuf, "-");
            addr = strtoul(temp, NULL, 16);
            break;
        }
    }
    bail:
        fclose(fp);
    return addr;
}

bool checkBreakPoint()
{
    __android_log_print(ANDROID_LOG_INFO,"JNI","13838438");
    int i, j;
    unsigned int base, offset, pheader;
    Elf32_Ehdr  *elfhdr;
    Elf32_Phdr *phdr;

    base = getLibAddr("libnative-lib.so");

    if (base == 0) {
        LOGI("get LibAddr failed");
        return false;
    }

    __android_log_print(ANDROID_LOG_INFO,"JNI","13838438");

    elfhdr = (Elf32_Ehdr *) base;
    pheader = base + elfhdr->e_phoff;

    for (i = 0; i < elfhdr->e_phnum; i ++) {
        phdr = (Elf32_Phdr*) (pheader + i *sizeof(Elf32_Phdr));

        if (!(phdr->p_flags & 1)) continue;
        offset = base + phdr->p_vaddr;
        offset += sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) * elfhdr->e_phnum;

        char *p = (char *)offset;
        for (j = 0; j < phdr->p_memsz; j ++) {
            if (*p == 0x01 && *(p + 1) == 0xde) {
                LOGI("Find thumb bpt %p", p);
                return true;
            } else if (*p == 0xf0 && *(p + 1) == 0xf7 && *(p + 2) == 0x00 && *(p + 3) == 0xa0) {
                LOGI("Find thumb2 bpt %p", p);
                return true;
            } else if (*p == 0x01 && *(p + 1) == 0x00 && *(p + 2) == 0x9f && *(p + 3) == 0xef) {
                LOGI("Find arm bpt %p", p);
                return true;
            }
            p ++;
        }
     }
    return false;
}



extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_stringFromJNI(
        JNIEnv* env,
        jclass clazz) {
    std::string str = "no one trace me:)\n";
    std::string hello;
    /*if(ptrace(PTRACE_TRACEME , 0, 0, 0) == -1)
    {
        str = "don't trace me :(";
        return env->NewStringUTF(str.c_str());;
    }*/



    return env->NewStringUTF(str.c_str());
}


extern "C" JNIEXPORT jstring JNICALL
        Java_com_tg_antiptrace_App_stringFromTime(JNIEnv* env,
                                                  jobject clazz){
    long start, end;
    start = clock();
    std::string hello = "Hello from time";
    end = clock();
    if (end - start > 10000)  {
        hello = "Debug from time";
    }
    return env->NewStringUTF(hello.c_str());
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_stringFromFile(JNIEnv* env,
                                          jobject clazz){
    std::string hello;
    std::stringstream  stream;
    int pid = getpid();
    int fd;
    stream << pid;
    stream >> hello;
    hello = "/proc/"  + hello + "/status";
    //LOGI(hello);
    char * pathname = new char[30];
    strcpy(pathname, hello.c_str());
    char * buf = new char[500];
    int flag = O_RDONLY;
    fd = open(pathname, flag);
    read(fd, buf, 500);
    char *tra = "TracePid";
    char *c = strstr(buf, tra);
    char *d = strstr(c, "\n");
    int length = d - c;
    strncpy(buf, c + 11, length - 11);
    buf[length - 11] = '\0';
    hello = buf;
    if (strcmp(buf, "0")) {
        hello = "Debug from file";
    } else {
        hello = "Hello from file";
    }
    close(fd);
    return env->NewStringUTF(hello.c_str());
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_stringFromTrick(JNIEnv* env,
                                          jobject clazz){
    std::string hello = "Hello from trick";
    return env->NewStringUTF(hello.c_str());
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_stringFromVm(JNIEnv* env,
                                           jobject clazz){
    std::string hello = "Hello from vm";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_stringFromPtrace(JNIEnv* env,
                                        jobject clazz){
    int check = ptrace(PTRACE_TRACEME, 0, 0, 0);
    LOGI("ret of ptrace : %d", check);
    std::string hello = "Hello from ptrace";
    if (check != 0) {
        hello = "Debug from ptrace";
    }

    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_stringFromBkpt(JNIEnv* env,
                                            jobject clazz){
    std::string hello = "Hello from bkpt";
    if (checkBreakPoint())
        hello = "Debug from bkpt";
    return env->NewStringUTF(hello.c_str());
}

char dynamic_ccode[] = {0x1f,0xb4, //push {r0-r4}
                        0x01,0xde, //breakpoint
                        0x1f,0xbc, //pop {r0-r4}
                        0xf7,0x46};//mov pc,lr

char *g_addr = 0;

void my_sigtrap(int sig){
    LOGI("my_sigtrap\n");
    char change_bkp[] = {0x00, 0x46};// mov r0,r0
    memcpy(g_addr + 2, change_bkp, 2);
    __builtin___clear_cache(g_addr,(g_addr+8)); // need to clear cache
    LOGI("change bpk to nop\n");
}

void anti4() {
    int ret, size;
    char *addr, tmpaddr;
    signal(SIGTRAP, my_sigtrap);
    addr = (char *)malloc(PAGE_SIZE * 2);

    memset(addr, 0, PAGE_SIZE * 2);
    g_addr = (char *)(((long) addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
    LOGI("addr: %p, g_addr : %p\n", addr, g_addr);

    ret = mprotect(g_addr, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (ret != 0) {
        LOGI("mprotect error\n");
        return;
    }

    size = 8;
    memcpy(g_addr, dynamic_ccode, size);
    __builtin___clear_cache(g_addr,(g_addr + size)); // need to clear cache
    LOGI("start stub\n");

    /*__asm__("push {r5}\n\t"
            "push {r0-r4,lr}\n\t"
            "mov r0,pc\n\t"  //此时pc指向后两条指令
            "add r0,r0,#6\n\t"//cjh:这里的add是add.w，所以会占32位，因此需要+6才对。 原文：+4 是的lr 地址为 pop{r0-r5}
            "mov lr,r0\n\t"
            "mov pc,%0\n\t"
            "pop {r0-r5}\n\t"
            "mov lr,r5\n\t" //恢复lr
            "pop {r5}\n\t"
    :
    :"r"(g_addr)
    :);*/
    LOGI("hi i'm here\n");
    free(addr);
    LOGI("hi, i'm here2\n");
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_stringFromSignal(JNIEnv* env,
                                          jobject clazz){
    anti4();
    std::string hello = "Hello from signal";
    return env->NewStringUTF(hello.c_str());
}

int pipefd[2];
int childpid;

void *anti3_thread(void *){
    int statue = -1, alive = 1, count = 0;
    close(pipefd[1]);

    while (read(pipefd[0], &statue, 4) > 0)
        break;
    sleep(1);

    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

    LOGI("pip-->read = %d", statue);

    while (true) {
        LOGI("pip--> statue = %d", statue);
        read(pipefd[0], &statue, 4);
        sleep(1);

        LOGI("pip--> statue2 = %d", statue);
        if (statue != 0) {
            kill(childpid, SIGKILL);
            kill(getpid(), SIGKILL);
            return NULL;
        }
        statue = -1;
    }
}

void anti3(){
    int pid, p;
    FILE *fd;
    char filename[MAX];
    char line[MAX];

    pid = getpid();
    sprintf(filename, "/proc/%d/status", pid);// 读取proc/pid/status中的TracePid
    p = fork();
    if (p == 0) {
        LOGI("Child");
        close(pipefd[0]);
        int pt, alive = 0;
        pt = ptrace(PTRACE_TRACEME, 0, 0, 0);// 子进程反调试
        while (true) {
            fd = fopen(filename, "r");
            while (fgets(line, MAX, fd)){
                if (strstr(line, "TracePid") != NULL) {
                    LOGI("line %s", line);
                    int statue = atoi(&line[10]);
                    LOGI("###### tracer pid:%d", statue);
                    write(pipefd[1], &statue, 4);

                    fclose(fd);

                    if (statue != 0) {
                        LOGI("###### tracer pid:%d", statue);
                        return;
                    }

                    break;
                }
            }
            sleep(1);
        }
    } else{
        LOGI("Father");
        childpid = p;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_tg_antiptrace_App_stringFromFork(JNIEnv* env,
                                          jobject clazz){
    std::string hello = "Hello from fork";
    pthread_t id_0;
    id_0 = pthread_self();
    pipe(pipefd);
    pthread_create(&id_0, NULL ,anti3_thread, (void *) NULL);
    LOGI("Start");
    anti3();


    return env->NewStringUTF(hello.c_str());
}

