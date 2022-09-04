/**
    \file JavaNativeMethod.hpp
    \brief Allocation and registration of java native methods in C++.
    \author Denis Sorokin
    \date 15.02.2016
*/

/**
* Cheat sheet:
*
* @code{.cpp}
*
* // Lets assume we have some global function:
* void testObjectWrapperAndStaticNativeMethodsEnd() { ... }
*
* // Let's register it as a static native function (old style / new style):
* jh::registerStaticNativeMethod<void>(Example::className(), "staticNativeMethod", &testObjectWrapperAndStaticNativeMethodsEnd);
* jh::registerStaticNativeMethod<Example, void>("staticNativeMethod", &testObjectWrapperAndStaticNativeMethodsEnd);
*
* @endcode
*/

#ifndef JH_JAVA_NATIVE_METHOD_HPP
#define JH_JAVA_NATIVE_METHOD_HPP

#include <jni.h>
#include "../core/ErrorHandler.hpp"
#include "../core/JavaMethodSignature.hpp"

namespace jh
{
    /**
    * Registers local functions as java native methods.
    *
    * @param javaClassName Name of the java class.
    * @param methodCount Total number of methods that should be registered by this call.
    * @param methodDescriptions Pointer to an array of java native method descriptions.
    *
    * @return True if methods were successfully registered or false otherwise.
    *
    * @warning Java native methods should be registered only after JNI initialization, i.e. dont use this method while doing some static-level stuff.
    * @warning Don't call native methods from java until they are 100% registered in C++. There will be a crash if they aren't.
    */
    bool registerJavaNativeMethods(std::string javaClassName, int methodCount, JNINativeMethod* methodDescriptions);

    /**
    * Registers some function or static method as a java native static method.
    *
    * @param javaClassName Name of the java class that has this native method.
    * @param methodName Name of the native method.
    * @param methodPointer Pointer to the local function or static method that will be called as java native method.
    *
    * @warning Java native methods should be registered only after JNI initialization, i.e. dont use this method while doing some static-level stuff.
    * @warning Don't call native methods from java until they are 100% registered in C++. There will be a crash if they aren't.
    */
    template<class ReturnType, class ... ArgumentTypes>
    bool registerStaticNativeMethod(std::string javaClassName, std::string methodName, ReturnType (*methodPointer)(ArgumentTypes...))
    {
        std::string signature = getJavaMethodSignature<ReturnType, ArgumentTypes...>();

        JNINativeMethod method[1] = {
            methodName.c_str(),
            signature.c_str(),
            (void*)methodPointer
        };

        return registerJavaNativeMethods(javaClassName, 1, method);
    }

    /**
    * Registers some function or static method as a java native static method.
    * Java class here is passed as a template argument, so there is no need
    * for an explicit java class name parameter.
    *
    * @param methodName Name of the native method.
    * @param methodPointer Pointer to the local function or static method that will be called as java native method.
    *
    * @warning Java native methods should be registered only after JNI initialization, i.e. dont use this method while doing some static-level stuff.
    * @warning Don't call native methods from java until they are 100% registered in C++. There will be a crash if they aren't.
    */
    template<class JavaClassType, class ReturnType, class ... ArgumentTypes>
    bool registerStaticNativeMethod(std::string methodName, ReturnType (*methodPointer)(ArgumentTypes...))
    {
        return registerStaticNativeMethod<ReturnType, ArgumentTypes ...>(JavaClassType::className(), methodName, methodPointer);
    }

    /**
    * This class provides an static method that should be registered as java native method.
    * It should not be used by the programmer itself, but by the JavaObjectWrapper class.
    *
    * @param id Dirty hack to distinguish two native methods with equal signatures. Should be unique for every native method per CppClass type.
    * @param CppClass C++ class that wraps the functionality of some Java class.
    * @param ReturnType The return type of java native method.
    * @param Arguments The types of arguments of java native method.
    *
    * @warning One should not forget to set the callback by method 'setCallback(...)' if using this class.
    * @warning Each combination of template parameters should correspond to ONLY ONE native method.
    * @warning Native methods that were implemented by this class should NOT be called inside the java object constructor.
    */
    template<int id, class CppClass, class ReturnType, class ... Arguments>
    class JavaNativeMethod
    {
        /**
        * The type of the class method that will be called as native method.
        */
        using MethodImplementationPointer = ReturnType(CppClass::*)(Arguments...);

        /**
        * Java object wrapper should freely access native method.
        */
        template <class, class>
        friend class JavaObjectWrapper;

    private:
        /**
        * Native method pointer is stored here.
        */
        static MethodImplementationPointer s_callback;

        /**
        * Sets the native method pointer. Should be called once (no less, no more) per native method.
        */
        static void setCallback(MethodImplementationPointer callback)
        {
            if (s_callback) {
                reportInternalError("redefining callback for java class [" + CppClass::JavaClass::className() + "]");
            }

            s_callback = callback;
        }

        /**
        * Static method that is used to link java native method and local instance method.
        */
        static ReturnType rawNativeMethod(JNIEnv*, jobject javaObject, Arguments ... args)
        {
            return CppClass::template callCppObjectMethod<ReturnType>(javaObject, [=] (CppClass* wrapperInstance) -> ReturnType {
                return (wrapperInstance->*s_callback)(args...);
            });
        }
    };

    template<int id, class CppClass, class ReturnType, class ... Arguments>
    typename JavaNativeMethod<id, CppClass, ReturnType, Arguments...>::MethodImplementationPointer JavaNativeMethod<id, CppClass, ReturnType, Arguments...>::s_callback(nullptr);
}

#endif
