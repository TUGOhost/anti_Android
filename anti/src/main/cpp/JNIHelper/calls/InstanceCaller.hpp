/**
    \file InstanceCaller.hpp
    \brief A utility to call java methods of a specific objects.
    \author Denis Sorokin
    \date 24.01.2016
*/

/**
* Cheat sheet:
*
* @code{.cpp}
*
* // Lets assume that we already have some Java object
* jobject someObject = ...;
*
* // Calling void method without arguments:
* jh::callMethod<void>(someObject, "voidMethodName");
*
* // Calling int method with two arguments:
* int sum = jh::callMethod<int, int, int>(someObject, "sumMethod", 4, 5);
*
* // Calling factory method that returns some custom Java object:
* JH_JAVA_CUSTOM_CLASS(Example, "com/class/path/Example");
* jobject newObject = jh::callMethod<Example, double>(someObject, "factoryMethodName", 3.1415);
*
* @endcode
*/

#ifndef JH_INSTANCE_CALLER_HPP
#define JH_INSTANCE_CALLER_HPP

#include <jni.h>
#include <string>
#include "../core/ErrorHandler.hpp"
#include "../core/JNIEnvironment.hpp"
#include "../core/JavaMethodSignature.hpp"

namespace jh
{
    /**
    * Class that can call methods which return java objects.
    */
    template<class ReturnType, class ... ArgumentTypes>
    struct InstanceCaller
    {
        static jobject call(JNIEnv* env, jobject instance, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallObjectMethod(instance, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call methods which doesn't return anything.
    */
    template<class ... ArgumentTypes>
    struct InstanceCaller<void, ArgumentTypes...>
    {
        static void call(JNIEnv* env, jobject instance, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            env->CallVoidMethod(instance, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call methods which return jboolean values.
    */
    template<class ... ArgumentTypes>
    struct InstanceCaller<jboolean, ArgumentTypes...>
    {
        static jboolean call(JNIEnv* env, jobject instance, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallBooleanMethod(instance, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call methods which return jint values.
    */
    template<class ... ArgumentTypes>
    struct InstanceCaller<jint, ArgumentTypes...>
    {
        static jint call(JNIEnv* env, jobject instance, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallIntMethod(instance, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call methods which return jlong values.
    */
    template<class ... ArgumentTypes>
    struct InstanceCaller<jlong, ArgumentTypes...>
    {
        static jlong call(JNIEnv* env, jobject instance, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallLongMethod(instance, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call methods which return jfloat values.
    */
    template<class ... ArgumentTypes>
    struct InstanceCaller<jfloat, ArgumentTypes...>
    {
        static jfloat call(JNIEnv* env, jobject instance, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallFloatMethod(instance, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call methods which return jdouble values.
    */
    template<class ... ArgumentTypes>
    struct InstanceCaller<jdouble, ArgumentTypes...>
    {
        static jdouble call(JNIEnv* env, jobject instance, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallDoubleMethod(instance, javaMethod, arguments...);
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
    template<class ReturnType, class ... ArgumentTypes>
    typename ToJavaType<ReturnType>::Type callMethod(jobject instance, std::string methodName, typename ToJavaType<ArgumentTypes>::Type ... arguments)
    {
        using RealReturnType = typename ToJavaType<ReturnType>::Type;

        JNIEnv* env = getCurrentJNIEnvironment();

        std::string methodSignature = getJavaMethodSignature<ReturnType, ArgumentTypes...>();

        jclass javaClass = env->GetObjectClass(instance);
        if (javaClass == nullptr) {
            reportInternalError("class for java object instance not found");
            return RealReturnType();
        }

        jmethodID javaMethod = env->GetMethodID(javaClass, methodName.c_str(), methodSignature.c_str());
        if (javaMethod == nullptr) {
            reportInternalError("method [" + methodName + "] for java object instance not found, tried signature [" + methodSignature + "]");
            return RealReturnType();
        }

        return static_cast<RealReturnType>(InstanceCaller<typename ToJavaType<ReturnType>::CallReturnType, typename ToJavaType<ArgumentTypes>::Type ...>::call(env, instance, javaMethod, arguments...));
    }
}

#endif
