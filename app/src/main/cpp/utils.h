//
// Created by tg on 2021/6/30.
//

/*
 * 工具程序
 * - 使用Android Toast打印回调信息
 * -
 *
 */

#ifndef ANTIPTRACE_UTILS_H
#define ANTIPTRACE_UTILS_H
void ToastUtil(JNIEnv *env, jobject object, char *message);

size_t strcat2(char **, ...);
#endif //ANTIPTRACE_UTILS_H
