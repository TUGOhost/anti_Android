//
// Created by tg on 2022/9/20.
//

#ifndef ANTI_ANDROID_UTILS_H
#define ANTI_ANDROID_UTILS_H

#include <android/log.h>

//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define TAG "ANTI"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG,__VA_ARGS__)

#endif //ANTI_ANDROID_UTILS_H
