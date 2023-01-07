#include "find_name.h"
#include <cstring>


#define NATIVE 0x00000100
#define STATIC 0x00000008

char *find_name::findField(C_JNIEnv *env, jobject clazz, int staticFlag, jclass type) {
    char *name = nullptr;
    jclass classClass = (*env)->FindClass((JNIEnv *) env, "java/lang/Class");
    jmethodID getDeclaredFields = (*env)->GetMethodID((JNIEnv *) env, classClass,
                                                      "getDeclaredFields",
                                                      "()[Ljava/lang/reflect/Field;");
    jclass classField = (*env)->FindClass((JNIEnv *) env, "java/lang/reflect/Field");
    jmethodID getModifiers = (*env)->GetMethodID((JNIEnv *) env, classField, "getModifiers", "()I");
    jmethodID getType = (*env)->GetMethodID((JNIEnv *) env, classField, "getType",
                                            "()Ljava/lang/Class;");
    jmethodID getName = (*env)->GetMethodID((JNIEnv *) env, classField, "getName",
                                            "()Ljava/lang/String;");
    auto fields = (jobjectArray) (*env)->CallObjectMethod((JNIEnv *) env, clazz, getDeclaredFields);
    int length = (*env)->GetArrayLength((JNIEnv *) env, fields);
    for (int i = 0; i < length; ++i) {
        jobject field = (*env)->GetObjectArrayElement((JNIEnv *) env, fields, i);
        int modifier = (*env)->CallIntMethod((JNIEnv *) env, field, getModifiers);
        if ((modifier & STATIC) == staticFlag
            && (*env)->IsSameObject((JNIEnv *) env, type,
                                    (*env)->CallObjectMethod((JNIEnv *) env, field, getType))) {
            auto fieldString = (jstring) (*env)->CallObjectMethod((JNIEnv *) env, field, getName);
            const char *fieldName = (*env)->GetStringUTFChars((JNIEnv *) env, fieldString, nullptr);
            name = strdup(fieldName);
            (*env)->ReleaseStringUTFChars((JNIEnv *) env, fieldString, fieldName);
            (*env)->DeleteLocalRef((JNIEnv *) env, fieldString);
        }
        (*env)->DeleteLocalRef((JNIEnv *) env, field);
        if (name != nullptr) {
            break;
        }
    }

    (*env)->DeleteLocalRef((JNIEnv *) env, fields);
    (*env)->DeleteLocalRef((JNIEnv *) env, classField);
    (*env)->DeleteLocalRef((JNIEnv *) env, classClass);
    return name;
}

char *find_name::findObjectArrayName(C_JNIEnv *env, jobject clazz) {
    jclass classObjectArray = (*env)->FindClass((JNIEnv *) env, "[Ljava/lang/Object;");
    char *name = findField(env, clazz, 0, classObjectArray);
    (*env)->DeleteLocalRef((JNIEnv *) env, classObjectArray);
    return name;
}

char *find_name::findStaticMapName(C_JNIEnv *env, jobject clazz) {
    jclass classMap = (*env)->FindClass((JNIEnv *) env, "java/util/Map");
    char *name = findField(env, clazz, STATIC, classMap);
    (*env)->DeleteLocalRef((JNIEnv *) env, classMap);
    return name;
}

char *find_name::findVoidStringName(C_JNIEnv *env, jclass clazz) {
    char *name = nullptr;
    jclass classClass = (*env)->FindClass((JNIEnv *) env, "java/lang/Class");
    jmethodID getDeclaredMethods = (*env)->GetMethodID((JNIEnv *) env, classClass,
                                                       "getDeclaredMethods",
                                                       "()[Ljava/lang/reflect/Method;");
    jclass classMethod = (*env)->FindClass((JNIEnv *) env, "java/lang/reflect/Method");
    jmethodID getModifiers = (*env)->GetMethodID((JNIEnv *) env, classMethod, "getModifiers",
                                                 "()I");
    jmethodID getParameterTypes = (*env)->GetMethodID((JNIEnv *) env, classMethod,
                                                      "getParameterTypes", "()[Ljava/lang/Class;");
    jmethodID getReturnType = (*env)->GetMethodID((JNIEnv *) env, classMethod, "getReturnType",
                                                  "()Ljava/lang/Class;");
    jmethodID getName = (*env)->GetMethodID((JNIEnv *) env, classMethod, "getName",
                                            "()Ljava/lang/String;");
    jclass classString = (*env)->FindClass((JNIEnv *) env, "java/lang/String");
    auto methods = (jobjectArray) (*env)->CallObjectMethod((JNIEnv *) env, clazz,
                                                           getDeclaredMethods);
    int length = (*env)->GetArrayLength((JNIEnv *) env, methods);
    for (int i = 0; i < length; ++i) {
        jobject method = (*env)->GetObjectArrayElement((JNIEnv *) env, methods, i);
        int modifier = (*env)->CallIntMethod((JNIEnv *) env, method, getModifiers);
        if ((modifier & (NATIVE | STATIC)) == STATIC
            && (*env)->GetArrayLength((JNIEnv *) env,
                                      (jarray) (*env)->CallObjectMethod((JNIEnv *) env, method,
                                                                        getParameterTypes)) == 0
            && (*env)->IsSameObject((JNIEnv *) env, classString,
                                    (*env)->CallObjectMethod((JNIEnv *) env, method,
                                                             getReturnType))) {
            auto methodString = (jstring) (*env)->CallObjectMethod((JNIEnv *) env, method, getName);
            const char *methodName = (*env)->GetStringUTFChars((JNIEnv *) env, methodString,
                                                               nullptr);
            jmethodID methodMethod = (*env)->GetStaticMethodID((JNIEnv *) env, clazz, methodName,
                                                               "()Ljava/lang/String;");
            auto bridgeString = (jstring) (*env)->CallStaticObjectMethod((JNIEnv *) env, clazz,
                                                                         methodMethod);
            if ((*env)->ExceptionCheck((JNIEnv *) env)) {
                (*env)->ExceptionClear((JNIEnv *) env);
            }
            if (bridgeString != nullptr) {
                const char *bridgeName = (*env)->GetStringUTFChars((JNIEnv *) env, bridgeString,
                                                                   nullptr);
                if (*bridgeName == 'L') {
                    name = strdup(bridgeName + 1);
                } else {
                    name = strdup(bridgeName);
                }
                (*env)->ReleaseStringUTFChars((JNIEnv *) env, bridgeString, bridgeName);
                (*env)->DeleteLocalRef((JNIEnv *) env, bridgeString);
            }
            (*env)->ReleaseStringUTFChars((JNIEnv *) env, methodString, methodName);
            (*env)->DeleteLocalRef((JNIEnv *) env, methodString);
        }
        (*env)->DeleteLocalRef((JNIEnv *) env, method);
        if (name != nullptr) {
            char *x = name;
            while (*x != ';' && *x != '\0') {
                x++;
            }
            *x = '\0';
            break;
        }
    }

    (*env)->DeleteLocalRef((JNIEnv *) env, methods);
    (*env)->DeleteLocalRef((JNIEnv *) env, classString);
    (*env)->DeleteLocalRef((JNIEnv *) env, classMethod);
    (*env)->DeleteLocalRef((JNIEnv *) env, classClass);
    return name;
}