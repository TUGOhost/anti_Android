#include "classloader.h"
#include "../JNIHelper/JNIHelper.hpp"
#include "art.h"
#include "plt.h"
#include "xposed.h"
#include "hash.h"
#include "Utils.h"

jobject newLocalRef(JNIEnv *env, void *object) {
    static jobject (*NewLocalRef)(JNIEnv *, void *) = nullptr;
    if (object == nullptr) {
        return nullptr;
    }
    if (NewLocalRef == nullptr) {
        NewLocalRef = (jobject (*)(JNIEnv *, void *)) plt_dlsym(
                "_ZN3art9JNIEnvExt11NewLocalRefEPNS_6mirror6ObjectE", nullptr);
        //LOGD("NewLocalRef: %p", NewLocalRef);
    }
    if (NewLocalRef != nullptr) {
        return NewLocalRef(env, object);
    } else {
        return nullptr;
    }
}

void deleteLocalRef(JNIEnv *env, jobject object) {
    static void (*DeleteLocalRef)(JNIEnv *, jobject) = nullptr;
    if (DeleteLocalRef == nullptr) {
        DeleteLocalRef = (void (*)(JNIEnv *, jobject)) plt_dlsym(
                "_ZN3art9JNIEnvExt14DeleteLocalRefEP8_jobject", nullptr);
        //LOGD("DeleteLocalRef: %p", DeleteLocalRef);
    }
    if (DeleteLocalRef != nullptr) {
        DeleteLocalRef(env, object);
    }
}

class ClassLoaderVisitor : public art::SingleRootVisitor {
public:
    ClassLoaderVisitor(C_JNIEnv *env, jclass classLoader) : env_(env), classLoader_(classLoader) {

    }

    void VisitRoot(art::mirror::Object *root, const art::RootInfo &info ATTRIBUTE_UNUSED) final {
        LOGE("in VisitRoot");
        jobject object = newLocalRef((JNIEnv *) env_, (jobject) root);
        if (object != nullptr) {
            LOGE("in VisitRoot object != nullptr");
            if ((*env_)->IsInstanceOf((JNIEnv *) env_, object, classLoader_)) {
                xposed::doAntiXposed(env_, object, (intptr_t) root);
            }
            deleteLocalRef((JNIEnv *) env_, object);
        }
    }

private:
    C_JNIEnv *env_;
    jclass classLoader_;
};

void classloader::checkGlobalRef(C_JNIEnv *env, jclass clazz) {
    auto VisitRoots = (void (*)(void *, void *)) plt_dlsym(
            "_ZN3art9JavaVMExt10VisitRootsEPNS_11RootVisitorE", nullptr);

    if (VisitRoots == nullptr) {
        return;
    }
    JavaVM *jvm;
    (*env)->GetJavaVM((JNIEnv *) env, &jvm);
    ClassLoaderVisitor visitor(env, clazz);
    VisitRoots(jvm, &visitor);
}

class WeakClassLoaderVisitor : public art::IsMarkedVisitor {
public:
    WeakClassLoaderVisitor(C_JNIEnv *env, jclass classLoader) : env_(env),
                                                                classLoader_(classLoader) {
    }

    art::mirror::Object *IsMarked(art::mirror::Object *obj) override {
        LOGE("in IsMarked");
        jobject object = newLocalRef((JNIEnv *) env_, (jobject) obj);
        if (object != nullptr) {
            LOGE("in IsMarked object != nullptr");
            if ((*env_)->IsInstanceOf((JNIEnv *) env_, object, classLoader_)) {
                xposed::doAntiXposed(env_, object, (intptr_t) obj);
            }
            deleteLocalRef((JNIEnv *) env_, object);
        }
        return obj;
    }

private:
    C_JNIEnv *env_;
    jclass classLoader_;
};

void classloader::checkWeakGlobalRef(C_JNIEnv *env, jclass clazz) {
    auto SweepJniWeakGlobals = (void (*)(void *, void *)) plt_dlsym(
            "_ZN3art9JavaVMExt19SweepJniWeakGlobalsEPNS_15IsMarkedVisitorE", nullptr);
    if (SweepJniWeakGlobals == nullptr) {
        return;
    }
    JavaVM *jvm;
    (*env)->GetJavaVM((JNIEnv *) env, &jvm);
    WeakClassLoaderVisitor visitor(env, clazz);
    SweepJniWeakGlobals(jvm, &visitor);
}

void classloader::checkClassLoader(C_JNIEnv *env, int sdk) {
    if (sdk < 21) {
        return;
    }

    jclass clazz = (*env)->FindClass((JNIEnv *) env, "dalvik/system/BaseDexClassLoader");
    if ((*env)->ExceptionCheck((JNIEnv *) env)) {
        (*env)->ExceptionClear((JNIEnv *) env);
    }

    if (clazz == nullptr) {
        return;
    }

    checkGlobalRef(env, clazz);
    checkWeakGlobalRef(env, clazz);

    clear();
    (*env)->DeleteLocalRef((JNIEnv *) env, clazz);

}