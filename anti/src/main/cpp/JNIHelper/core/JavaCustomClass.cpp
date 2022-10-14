#include <jni.h>
#include "JavaCustomClass.hpp"
#include "JNIEnvironment.hpp"
#include "../utils/JniGlobalRef.hpp"
#include "../calls/InstanceCaller.hpp"
#include "JavaMethod.h"


JAVA_CLASS(JavaClassLoader, "java/lang/ClassLoader")

JAVA_CLASS(JavaClass, "java/lang/Class")

namespace jh {

    static JniGlobalRef<jobject> *gClassLoader;
    static jmethodID gClassLoaderFindClass;

    jclass FindClass(const std::string &name) {
        JNIEnv *env = getCurrentJNIEnvironment();
        jclass javaClass = env->FindClass(name.c_str());
        if (javaClass == nullptr) {
            env->ExceptionClear();
            javaClass = static_cast<jclass>(env->CallObjectMethod(gClassLoader->get(),
                                                                  gClassLoaderFindClass,
                                                                  env->NewStringUTF(name.c_str())));
        }
        return javaClass;
    }

    void loadClassLoader() {
        if (gClassLoader == nullptr) {
            jobject classloader = jh::callMethod<JavaClassLoader>(getCurrentApplication(),
                                                                  "getClassLoader",
                                                                  true);
            gClassLoader = new JniGlobalRef<jobject>();
            gClassLoader->set(classloader);
            gClassLoaderFindClass = findMethod<JavaClass, jstring>(JavaClassLoader::className(),
                                                                   "findClass");
        }
    }
}