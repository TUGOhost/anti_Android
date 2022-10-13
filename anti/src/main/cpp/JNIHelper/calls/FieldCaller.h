#include "jni.h"
#include "string.h"

#ifndef ANTI_ANDROID_FIELDCALLER_H
#define ANTI_ANDROID_FIELDCALLER_H
namespace jh {
    /**
       * Class that can call methods which return java objects.
       */
    template<class ReturnType>
    struct FiledCaller {
        static jobject
        call(JNIEnv *env, jobject instance, jfieldID javaMethod, bool autoClearException) {
            jobject result = env->GetObjectField(instance, javaMethod);

            return result;
        }
    };

    /**
* Calls a method of some Java object instance. Programmer should explicitly
* specify the return type and argument types via template arguments.
*
* @param instance Java object (jobject)
* @param methodName Method name as string.
* @param arguments List of arguments to the java method call.
* @return Some value of ReturnType type returned by the specified method.
*/
    template<class ReturnType>
    typename ToJavaType<ReturnType>::Type
    getFiled(jobject instance, std::string fieldName, bool autoClearException) {
        using RealReturnType = typename ToJavaType<ReturnType>::Type;
        if (instance == nullptr)
            return ToJavaType<ReturnType>::initValue();
        JNIEnv *env = getCurrentJNIEnvironment();

        jclass javaClass = env->GetObjectClass(instance);
        if (javaClass == nullptr) {
            reportInternalError("class for java object instance not found");
            return ToJavaType<ReturnType>::initValue();
        }

        jfieldID javaMethod = env->GetFieldID(javaClass, fieldName.c_str(),
                                              ToJavaType<ReturnType>::signature().c_str());
        if (javaMethod == nullptr) {
            reportInternalError("filed [" + fieldName + "] for java object instance not found");
            return ToJavaType<ReturnType>::initValue();
        }
        env->DeleteLocalRef(javaClass);
        return static_cast<RealReturnType>(FiledCaller<typename ToJavaType<ReturnType>::CallReturnType>::call(
                env, instance, javaMethod, autoClearException));
    }
}
#endif //ANTI_ANDROID_FIELDCALLER_H
