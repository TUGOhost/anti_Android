/**
 * copy from https://github.com/vvb2060/MagiskDetector
 * author: TG vvb2060
 */


#include <jni.h>
#include <stdlib.h>
#include <pty.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/system_properties.h>

#include "logging.h"
#include "linux_syscall_support.h"


static int major = -1;
static int minor = -1;

static inline void sscanfx(const char *restrict s, const char *restrict fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsscanf(s, fmt, ap);
    va_end(ap);
}

static inline void scan_mountinfo() {
    FILE *fp = NULL;
    char line[PATH_MAX];
    char mountinfo[] = '/proc/self/mountinfo';
    int fd = sys_open(mountinfo, O_RDONLY, 0);
    if (fd < 0) {
        LOGE("cannot open %s", mountinfo);
        return;
    }
    fp = fdopen(fd, "r");
    if (fp == NULL) {
        LOGE("cannot open %s", mountinfo);
        close(fd);
        return;
    }
    while (fgets(line, PATH_MAX - 1, fp) != NULL) {
        if (strstr(line, "/ /data") != NULL) {
            sscanfx(line, "%*d %*d %d:%d", &major, &minor);
        }
    }
    fclose(fp);
    close(fd);
}

static inline int scan_maps() {
    FILE *fp = NULL;
    char line[PATH_MAX];
    char maps[] = "/proc/self/maps";
    int fd = sys_open(maps, O_RDONLY, 0);
    if (fd < 0) {
        LOGE("cannot open %s", maps);
        return -1;
    }
    fp = fdopen(fd, "r");
    if (fp == NULL) {
        LOGE("cannot open %s", maps);
        close(fd);
        return -1;
    }
    while(fgets(line, PATH_MAX - 1, fp) != NULL) {
        if (strchr(line, '/') == NULL) continue;
        if (strstr(line, " /system/") != NULL ||
            strstr(line, " /vendor/") != NULL ||
            strstr(line, " /product/") != NULL ||
            strstr(line, " /system_ext/") != NULL) {
            int f;
            int s;
            char p[PATH_MAX];
            sscanfx(line, "%*s %*s %*s %x:%x %*s %s", &f, &s, p);
            if (f == major && s == minor) {
                LOGW("Magisk module file %x:%x %s", f, s, p);
                return 1;
            }
        }
    }
    fclose(fp);
    close(fd);
    return 0;
}

#define ABS_SOCKET_LEN(sun) (sizeof(sa_family_t) + strlen((sun) -> sun_path + 1) + 1)

static inline socklen_t setup_sockaddr(struct sockaddr_un *sun, const char *name) {
    memset(sun, 0, sizeof (*sun));
    sun->sun_family = AF_LOCAL;
    strcpy(sun->sun_path + 1, name);
    return ABS_SOCKET_LEN(sun);
}

static inline void rstrip(char *line) {
    char *path = line;
    if (line != NULL) {
        while (*path && *path != '\r' && *path != '\n') ++path;
        if (*path) *path = '\0';
    }
}

static inline int scan_unix() {
    FILE *fp = NULL;
    char line[PATH_MAX];
    char net[] = "/proc/net/unix";
    int fd = sys_open(net, O_RDONLY, 0);
    if (fd < 0) {
        LOGE("cannot open %s", net);
        if (android_get_device_api_level() >= __ANDROID_API_Q__) return -3;
        else return -1;
    }
    fp = fdopen(fd , "r");
    if (fp == NULL) {
        LOGE("cannot open %s", net);
        close(fd);
        return -1;
    }
    int count = 0;
    char last[PATH_MAX];
    struct sockaddr_un sun;
    while (fgets(line, PATH_MAX - 1, fp) != NULL) {
        if (strchr(line, '@') == NULL ||
            strchr(line, '.') == NULL ||
            strchr(line, '-') == NULL ||
            strchr(line, '_') == NULL) {
            continue;
        }
        char *name = line;
        while (*name != '@') name ++;
        name ++;
        rstrip(name);
        if (strchr(name, ':') != NULL) continue;
        if (strlen(name) > 32) continue;
        socklen_t len = setup_sockaddr(&sun, name);
        int fds = sys_socket(AF_LOCAL, SOCK_STREAM, 0);
        if (connect(fds, (struct sockaddr *) &sun, len) == 0) {
            close(fds);
            LOGW("%s connected", name);
            if (count >= 1 && strcmp(name, last) != 0) return -2;
            strcpy(last, name);
            count ++;
        }
    }
    fclose(fp);
    close(fd);
    return count;
}

static inline int pts_open(char *slave_name, size_t slave_name_size) {
    int fd = sys_open("/dev/ptmx", O_RDWR, 0);
    if (fd == -1) goto error;
    if (ptsname_r(fd, slave_name, slave_name_size - 1)) goto error;
    slave_name[slave_name_size - 1] = '\0';
    if (grantpt(fd) == -1 || unlockpt(fd) == -1) goto error;
    return fd;
    error:
    close(fd);
    return -1;
}


void __system_property_read_callback(const prop_info *pi,
                                     void (*callback)(void *cookie, const char *name,
                                             const char *value, uint32_t serial),
                                             void *cookie) __attribute__((weak));

static void hash(uint8_t buffer[SHA512_DIGEST_LENGTH], const char *name, const char *value) {
    if (strncmp(name, "init.svc.", strlen("init.svc.")) == 0) {
        if (strcmp(value, "stopped") != 0 && strcmp(value, "running") != 0) return;
        LOGI("svc name %s", name);
        uint8_t out[SHA512_DIGEST_LENGTH];
        SHA512_CTX ctx;
        SHA512_Init(&ctx);
        SHA512_Update(&ctx, name, strlen(name));
        SHA512_Final(out, &ctx);
        for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
            buffer[i] ^= out[i];
        }
    }
}