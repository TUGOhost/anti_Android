/**
    \file JNIEnvironment.cpp
    \brief Utilities to get and use JNIEnv* pointer.
    \author Denis Sorokin
    \date 24.01.2016
*/

#include <jni.h>
//#include "../../zframework/core/_android/jnienv.h"
#include "../../anti.h"
#include "../core/JNIEnvironment.hpp"
#include "../core/ErrorHandler.hpp"
#include "../calls/StaticCaller.hpp"
#include "../calls/InstanceCaller.hpp"
#include "../utils/JniGlobalRef.hpp"


JAVA_CLASS(JavaActivityThread, "android/app/ActivityThread")

JAVA_CLASS(JavaApplication, "android/app/Application")

namespace jh {
    static JniGlobalRef<jobject> *globalActivityThread;
    static JniGlobalRef<jobject> *globalApplication;

    static JavaVM *jvm;

    void onLoad(JavaVM *vm) {
        jvm = vm;
        if (globalActivityThread == nullptr)
            globalActivityThread = new JniGlobalRef<jobject>();
        if (globalApplication == nullptr)
            globalApplication = new JniGlobalRef<jobject>();
        jobject activityThread = jh::callStaticMethod<JavaActivityThread, JavaActivityThread>(
                "currentActivityThread");
        globalActivityThread->set(activityThread);
        jobject application = jh::callMethod<JavaApplication>(activityThread, "getApplication",
                                                              true);
        globalApplication->set(application);
        loadClassLoader();

    }

    JavaVM *getJavaVM() {
        return jvm;
    }

    jobject getCurrentApplication() {
        return globalApplication->get();
    }

    JNIEnv *getCurrentJNIEnvironment() {
        JNIEnv *env = nullptr;
        JavaVM *javaVM = getJavaVM();

        javaVM->GetEnv((void **) &env, JNI_VERSION_1_6);

        if (env == nullptr) {
            reportInternalError("jni environment not found");
        }

        return env;
    }

    JNIEnvironmentGuarantee::JNIEnvironmentGuarantee()
            : m_threadShouldBeDetached(false) {
        JNIEnv *env = nullptr;
        JavaVM *javaVM = getJavaVM();

        int getEnvStatus = javaVM->GetEnv((void **) &env, JNI_VERSION_1_6);

        if (getEnvStatus == JNI_EDETACHED) {
            if (javaVM->AttachCurrentThread(&env, nullptr) != 0) {
                reportInternalError("couldn't attach current thread to java VM");
            } else {
                m_threadShouldBeDetached = true;
                // no classes besides the system ones!
                // TODO : do something with this
            }
        }

        if (env == nullptr) {
            reportInternalError("couldn't get jni environment for current thread");
        }
    }

    JNIEnvironmentGuarantee::~JNIEnvironmentGuarantee() {
        if (m_threadShouldBeDetached) {
            getJavaVM()->DetachCurrentThread();
        }
    }
}
