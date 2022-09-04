/**
    \file StaticCaller.hpp
    \brief Static java methods calls.
    \author Denis Sorokin
    \date 24.01.2016
*/

/**
* Cheat sheet:
*
* @code{.cpp}
*
* // Declaring some custom Java class:
* JH_JAVA_CUSTOM_CLASS(Example, "com/class/path/Example");
*
* // Calling void static method without arguments (old style / new style):
* jh::callStaticMethod<void>(Example::className(), "voidMethodName");
* jh::callStaticMethod<Example, void>("voidMethodName");
*
* // Calling int static method with two arguments (old style / new style):
* int sum = jh::callStaticMethod<int, int, int>(Example::className(), "sumMethod", 4, 5);
* int sum = jh::callStaticMethod<Example, int, int, int>("sumMethod", 4, 5);
*
* // Calling static factory method that returns some custom Java object (old style / new style):
* jobject newObject = jh::callStaticMethod<Example, double>(Example::className(), "factoryMethodName", 3.1415);
* jobject newObject = jh::callStaticMethod<Example, Example, double>("factoryMethodName", 3.1415);
*
* @endcode
*/

#ifndef JH_STATIC_CALLER_HPP
#define JH_STATIC_CALLER_HPP

#include <jni.h>
#include <string>
#include "../core/ToJavaType.hpp"
#include "../core/ErrorHandler.hpp"
#include "../core/JNIEnvironment.hpp"
#include "../core/JavaMethodSignature.hpp"

namespace jh
{
    /**
    * Class that can call static methods which return java objects.
    */
    template<class ReturnType, class ... ArgumentTypes>
    struct StaticCaller
    {
        static jobject call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticObjectMethod(javaClass, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call static methods which doesn't return anything.
    */
    template<class ... ArgumentTypes>
    struct StaticCaller<void, ArgumentTypes...>
    {
        static void call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            env->CallStaticVoidMethod(javaClass, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call static methods which return jboolean values.
    */
    template<class ... ArgumentTypes>
    struct StaticCaller<jboolean, ArgumentTypes...>
    {
        static jboolean call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticBooleanMethod(javaClass, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call static methods which return jint values.
    */
    template<class ... ArgumentTypes>
    struct StaticCaller<jint, ArgumentTypes...>
    {
        static jint call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticIntMethod(javaClass, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call static methods which return jlong values.
    */
    template<class ... ArgumentTypes>
    struct StaticCaller<jlong, ArgumentTypes...>
    {
        static jlong call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticLongMethod(javaClass, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call static methods which return jfloat values.
    */
    template<class ... ArgumentTypes>
    struct StaticCaller<jfloat, ArgumentTypes...>
    {
        static jfloat call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticFloatMethod(javaClass, javaMethod, arguments...);
        }
    };

    /**
    * Class that can call static methods which return jdouble values.
    */
    template<class ... ArgumentTypes>
    struct StaticCaller<jdouble, ArgumentTypes...>
    {
        static jdouble call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticDoubleMethod(javaClass, javaMethod, arguments...);
        }
    };

    /**
    * Calls a static method of some Java class. Programmer should explicitly
    * specify the return type and argument types via template arguments.
    *
    * @param className Java class name as string.
    * @param methodName Method name as string.
    * @param arguments List of arguments to the java method call.
    * @return Some value of ReturnType type returned by the specified static method.
    */
    template<class ReturnType, class ... ArgumentTypes>
    typename ToJavaType<ReturnType>::Type callStaticMethod(std::string className, std::string methodName, typename ToJavaType<ArgumentTypes>::Type ... arguments)
    {
        using RealReturnType = typename ToJavaType<ReturnType>::Type;

        JNIEnv* env = getCurrentJNIEnvironment();

        std::string methodSignature = getJavaMethodSignature<ReturnType, ArgumentTypes...>();

        jclass javaClass = env->FindClass(className.c_str());
        if (javaClass == nullptr) {
            reportInternalError("class not found [" + className + "]");
            return RealReturnType();
        }

        jmethodID javaMethod = env->GetStaticMethodID(javaClass, methodName.c_str(), methodSignature.c_str());
        if (javaMethod == nullptr) {
            reportInternalError("method [" + methodName + "] for class [" + className + "] not found, tried signature [" + methodSignature + "]");
            return RealReturnType();
        }

        return static_cast<RealReturnType>(StaticCaller<typename ToJavaType<ReturnType>::CallReturnType, typename ToJavaType<ArgumentTypes>::Type ...>::call(env, javaClass, javaMethod, arguments...));
    }

    /**
    * New style of calling static methods, where java class is specified
    * via template argument.
    *
    * @param methodName Method name as string.
    * @param arguments List of arguments to the java method call.
    * @return Some value of ReturnType type returned by the specified static method.
    */
    template<class JavaClassType, class ReturnType, class ... ArgumentTypes>
    typename ToJavaType<ReturnType>::Type callStaticMethod(std::string methodName, typename ToJavaType<ArgumentTypes>::Type ... arguments)
    {
        return callStaticMethod<ReturnType, ArgumentTypes ...>(JavaClassType::className(), methodName, arguments...);
    }
}

#endif
