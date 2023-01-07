#include <string>
#include "xposed.h"
#include "find_name.h"
#include "hash.h"
#include "../check/anti_xposed.h"
#include "Utils.h"
#include "../JNIHelper/JNIHelper.hpp"

jclass xposed::findLoadedClass(C_JNIEnv *env, jobject classLoader, const char *name) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;

    jclass loadedClass = nullptr;
    jmethodID findLoadedClass;
    jstring string;

    jclass vmClassLoader = (*env)->FindClass((JNIEnv *) env, "java/lang/VMClassLoader");
    if ((*env)->ExceptionCheck((JNIEnv *) env)) {
        (*env)->ExceptionClear((JNIEnv *) env);
    }
    if (vmClassLoader == nullptr) {
        goto clean;
    }

    findLoadedClass = (*env)->GetStaticMethodID((JNIEnv *) env, vmClassLoader, "findLoadedClass",
                                                "(Ljava/lang/ClassLoader;Ljava/lang/String;)Ljava/lang/Class;");
    if ((*env)->ExceptionCheck((JNIEnv *) env)) {
        (*env)->ExceptionClear((JNIEnv *) env);
    }
    if (findLoadedClass == nullptr) {
        goto cleanVmClassLoader;
    }

    //string = (*env)->NewStringUTF((JNIEnv *) env, name);
    string = jh::createJString(name);
    loadedClass = (jclass) (*env)->CallStaticObjectMethod((JNIEnv *) env, vmClassLoader,
                                                          findLoadedClass, classLoader, string);

    if ((*env)->ExceptionCheck((JNIEnv *) env)) {
        (*env)->ExceptionClear((JNIEnv *) env);
    }

    if (loadedClass == nullptr) {
        LOGE("loadedClass == nullptr");
    }

    (*env)->DeleteLocalRef((JNIEnv *) env, string);
    cleanVmClassLoader:
    (*env)->DeleteLocalRef((JNIEnv *) env, vmClassLoader);
    clean:
    return loadedClass;
}


jclass xposed::findXposedBridge(C_JNIEnv *env, jobject classLoader) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    return findLoadedClass(env, classLoader, "de/robv/android/xposed/XposedBridge");
}

jclass xposed::findXposedHelper(C_JNIEnv *env, jobject classLoader) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    return findLoadedClass(env, classLoader, "de/robv/android/xposed/XposedHelpers");
}

bool xposed::disableXposedBridge(C_JNIEnv *env, jclass classXposedBridge) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    jfieldID field = (*env)->GetStaticFieldID((JNIEnv *) env, classXposedBridge, "disableHooks",
                                              "Z");
    if ((*env)->ExceptionCheck((JNIEnv *) env)) {
        (*env)->ExceptionClear((JNIEnv *) env);
    }
    if (field == nullptr) {
        return false;
    }
    (*env)->SetStaticBooleanField((JNIEnv *) env, classXposedBridge, field, JNI_TRUE);
    if ((*env)->ExceptionCheck((JNIEnv *) env)) {
        (*env)->ExceptionClear((JNIEnv *) env);
        return false;
    }
    return true;
}

jfieldID xposed::findMapField(C_JNIEnv *env, jclass classXposedBridge) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    std::string signature = "Ljava/util/Map;";
    jfieldID field = (*env)->GetStaticFieldID((JNIEnv *) env, classXposedBridge,
                                              "sHookedMethodCallbacks", signature.c_str());
    if ((*env)->ExceptionCheck((JNIEnv *) env)) {
        (*env)->ExceptionClear((JNIEnv *) env);
    }
    if (field != nullptr) {
        return field;
    }
    const char *mapName = find_name::findStaticMapName(env, classXposedBridge);
    if (mapName != nullptr) {
#ifdef DEBUG
        LOGI("mapName: %s", mapName);
#endif
        return (*env)->GetStaticFieldID((JNIEnv *) env, classXposedBridge, mapName,
                                        signature.c_str());
    }
    return nullptr;
}

bool xposed::doClearHooksClass(C_JNIEnv *env, jclass classXposedBridge) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    bool antied = false;

    jfieldID field = findMapField(env, classXposedBridge);
    if (field == nullptr) {
        return false;
    }
    jobject map = (*env)->GetStaticObjectField((JNIEnv *) env, classXposedBridge, field);

    jclass classMap = (*env)->FindClass((JNIEnv *) env, "java/util/HashMap");
    if (map == nullptr || !(*env)->IsInstanceOf((JNIEnv *) env, map, classMap)) {
        return false;
    }

    jmethodID method = (*env)->GetMethodID((JNIEnv *) env, classMap, "isEmpty", "()Z");
    jboolean isEmpty = (*env)->CallBooleanMethod((JNIEnv *) env, map, method);
    if (isEmpty == JNI_TRUE) {
        isEmpty = (*env)->CallNonvirtualBooleanMethod((JNIEnv *) env, map,
                                                      (*env)->GetObjectClass((JNIEnv *) env, map),
                                                      method);
    }

    jobject values;
    method = (*env)->GetMethodID((JNIEnv *) env, classMap, "values", "()Ljava/util/Collection;");
    if (isEmpty == JNI_TRUE) {
        values = (*env)->CallObjectMethod((JNIEnv *) env, map, method);
    } else {
        values = (*env)->CallNonvirtualObjectMethod((JNIEnv *) env, map,
                                                    (*env)->GetObjectClass((JNIEnv *) env, map),
                                                    method);
    }

    jclass classCollection = (*env)->FindClass((JNIEnv *) env, "java/util/Collection");
    method = (*env)->GetMethodID((JNIEnv *) env, classCollection, "iterator",
                                 "()Ljava/util/Iterator;");
    jobject iterator = (*env)->CallObjectMethod((JNIEnv *) env, values, method);

    jclass classIterator = (*env)->FindClass((JNIEnv *) env, "java/util/Iterator");
    jmethodID hasNext = (*env)->GetMethodID((JNIEnv *) env, classIterator, "hasNext", "()Z");
    jmethodID next = (*env)->GetMethodID((JNIEnv *) env, classIterator, "next",
                                         "()Ljava/lang/Object;");

    jclass classObject = (*env)->FindClass((JNIEnv *) env, "java/lang/Object");
    jobject emptyArray = (*env)->NewObjectArray((JNIEnv *) env, 0, classObject, nullptr);
    jfieldID fieldElements = nullptr;
    while ((*env)->CallBooleanMethod((JNIEnv *) env, iterator, hasNext)) {
        jobject hook = (*env)->CallObjectMethod((JNIEnv *) env, iterator, next);
        if (hook == nullptr) {
            continue;
        }
        if (fieldElements == nullptr) {
            jclass hookClass = (*env)->GetObjectClass((JNIEnv *) env, hook);
            char *name = find_name::findObjectArrayName(env, hookClass);
            if (name == nullptr) {
                (*env)->DeleteLocalRef((JNIEnv *) env, hookClass);
                break;
            }
            fieldElements = (*env)->GetFieldID((JNIEnv *) env, hookClass, name,
                                               "[Ljava/lang/Object;");
            free(name);
            (*env)->DeleteLocalRef((JNIEnv *) env, hookClass);
        }
        (*env)->SetObjectField((JNIEnv *) env, hook, fieldElements, emptyArray);
        if ((*env)->ExceptionCheck((JNIEnv *) env)) {
            (*env)->ExceptionClear((JNIEnv *) env);
        } else if (!antied) {
            antied = true;
        }
        (*env)->DeleteLocalRef((JNIEnv *) env, hook);
    }

    (*env)->DeleteLocalRef((JNIEnv *) env, classIterator);
    (*env)->DeleteLocalRef((JNIEnv *) env, iterator);
    (*env)->DeleteLocalRef((JNIEnv *) env, classCollection);
    (*env)->DeleteLocalRef((JNIEnv *) env, values);
    (*env)->DeleteLocalRef((JNIEnv *) env, classMap);
    (*env)->DeleteLocalRef((JNIEnv *) env, map);

    return antied;
}

bool xposed::doClearHooksCommon(C_JNIEnv *env, jobject classLoader, const char *name) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    bool antied = false;
    jclass classXposedBridge = findLoadedClass(env, classLoader, name);
    if (classXposedBridge != nullptr) {
        antied = doClearHooksClass(env, (jclass) classXposedBridge);
        (*env)->DeleteLocalRef((JNIEnv *) env, classXposedBridge);
    }
    return antied;
}

bool xposed::clearHooks(C_JNIEnv *env, jobject classLoader) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    bool antied = false;


    jclass epicNative = findLoadedClass(env, classLoader, "me/weishu/epic/art2/EpicNative");
    if (epicNative != nullptr) {
        char *bridgeClass = find_name::findVoidStringName(env, epicNative);

        if (bridgeClass != nullptr) {
            antied |= doClearHooksCommon(env, classLoader, bridgeClass);
            free(bridgeClass);
        }
        (*env)->DeleteLocalRef((JNIEnv *) env, epicNative);
    }
    if (!antied) {
        antied |= doClearHooksCommon(env, classLoader, "me/weishu/epic/art2/EpicBridge");
    }
    if (!antied) {
        antied |= doClearHooksCommon(env, classLoader, "de/robv/android/xposed/DexposedBridge");
    }
    if (!antied) {
        antied |= doClearHooksCommon(env, classLoader, "de/robv/android/xposed/XposedBridge");
    }

    return antied;
}


void xposed::doAntiXposed(C_JNIEnv *env, jobject object, intptr_t hash) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    if (!add(hash)) {
        return;
    }

    jclass classXposedBridge = findXposedBridge(env, object);
    jclass classXposedHelper = findXposedHelper(env, object);

    if (classXposedBridge != nullptr || classXposedHelper != nullptr) {
        LOGE("classXposedBridge != nullptr || classXposedHelper != nullptr");
        if (xposed_status == NO_XPOSED) {
            LOGE("::xposed_status == NO_XPOSED");
            xposed_status = FOUND_XPOSED;
        }

        disableXposedBridge(env, classXposedBridge);
        if (clearHooks(env, object)) {
            if (xposed_status < ANTIED_XPOSED) {
                xposed_status = ANTIED_XPOSED;
            }
        }
    } else {
        LOGE("classXposedBridge = nullptr || classXposedHelper == nullptr");
        return;
    }
}

void xposed::checkCallStack(C_JNIEnv *env) {
    jh::JNIEnvironmentGuarantee jniEnvironmentGuarantee;
    jclass threadClass = (*env)->FindClass((JNIEnv *) env, "java/lang/Thread");
    jmethodID currentThread = (*env)->GetStaticMethodID((JNIEnv *) env, threadClass,
                                                        "currentThread", "()Ljava/lang/Thread;");
    jmethodID getStackTrace = (*env)->GetMethodID((JNIEnv *) env, threadClass, "getStackTrace",
                                                  "()[Ljava/lang/StackTraceElement;");
    jclass StackTraceElementClass = (*env)->FindClass((JNIEnv *) env,
                                                      "java/lang/StackTraceElement");
    jmethodID getClassName = (*env)->GetMethodID((JNIEnv *) env, StackTraceElementClass,
                                                 "getClassName", "()Ljava/lang/String;");

    jobject thread = (*env)->CallStaticObjectMethod((JNIEnv *) env, threadClass, currentThread);
    auto stackTraces = (jobjectArray) (*env)->CallObjectMethod((JNIEnv *) env, thread,
                                                               getStackTrace);
    int length = (*env)->GetArrayLength((JNIEnv *) env, stackTraces);
    for (int i = 0; i < length; i++) {
        jobject stackTrace = (*env)->GetObjectArrayElement((JNIEnv *) env, stackTraces, i);
        auto jclassName = (jstring) (*env)->CallObjectMethod((JNIEnv *) env, stackTrace,
                                                             getClassName);
        const char *className = (*env)->GetStringUTFChars((JNIEnv *) env, jclassName, nullptr);
        std::string methodHook = "de.robv.android.xposed.XC_MethodHook";
        if (memcmp(className, methodHook.c_str(), methodHook.size()) == 0) {
            xposed_status = CAN_NOT_ANTI_XPOSED;
        }
        (*env)->ReleaseStringUTFChars((JNIEnv *) env, jclassName, className);
        if (xposed_status == CAN_NOT_ANTI_XPOSED) break;
    }
}


