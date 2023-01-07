#include "../calls/InstanceCaller.hpp"
#include "../native/JavaObjectWrapper.hpp"
#include "jni.h"
#include "../calls/FieldCaller.h"

JAVA_CLASS(JavaApplication,
           "android/app/Application")

JAVA_CLASS(JavaContext,
           "android/content/Context")

JAVA_CLASS(JavaApplicationInfo,
           "android/content/pm/ApplicationInfo")

#ifndef ANTI_ANDROID_ACTIVITYTHREAD_H
#define ANTI_ANDROID_ACTIVITYTHREAD_H

class Application : public jh::JavaObjectWrapper<JavaApplication, Application> {
public:
    void linkJavaNativeMethods() override {

    }

    jobject initializeJavaObject() override {
        return jh::getCurrentApplication();
    }

    jobject getContext() {
        //return object();
        return jh::callMethod<JavaContext>(object(), "getApplicationContext", true);
    }

    jstring getDataDir() {
        jobject applicationInfo = jh::callMethod<JavaApplicationInfo>(getContext(),
                                                                      "getApplicationInfo",
                                                                      true);

        jstring ret = jh::getFiled<jstring>(applicationInfo, "dataDir", true);
        jh::getCurrentJNIEnvironment()->DeleteLocalRef(applicationInfo);
        return ret;
    }
};

#endif //ANTI_ANDROID_ACTIVITYTHREAD_H
