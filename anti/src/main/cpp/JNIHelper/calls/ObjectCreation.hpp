/**
    \file ObjectCreation.hpp
    \brief Creation of new java objects from C++ code.
    \author Denis Sorokin
    \date 24.01.2016
*/

/**
* Cheat sheet:
*
* @code{.cpp}
*
* // Lets declare custom Java class:
* JH_JAVA_CUSTOM_CLASS(Example, "com/class/path/Example");
* JH_JAVA_CUSTOM_CLASS(Parameter, "com/class/path/Parameter");
*
* // Create an object via default constructor (old style / new style):
* jobject first    = jh::createNewObject<>(Parameter::className());
* jobject first_v2 = jh::createNewObject<Parameter>();
*
* // Create an object via constructor with two arguments (old style / new style):
* jobject second    = jh::createNewObject<Parameter, int>(Example::className(), first, 100);
* jobject second_v2 = jh::createNewObject<Example, Parameter, int>(first, 100);
*
* @endcode
*/

#ifndef JH_OBJECT_CREATION_HPP
#define JH_OBJECT_CREATION_HPP

#include <jni.h>
#include <string>
#include "../core/ToJavaType.hpp"
#include "../core/ErrorHandler.hpp"
#include "../core/JNIEnvironment.hpp"
#include "../core/JavaMethodSignature.hpp"

namespace jh
{
    /**
    * Allows to create Java objects via their constructors. Constructor argument
    * types should be explicitly specified via template arguments.
    *
    * @param className Java class name as a string.
    * @param arguments List of arguments for the constructor.
    * @return Create Java object pointer (aka jobject).
    */
    template<class ... ArgumentTypes>
    jobject createNewObject(std::string className, typename ToJavaType<ArgumentTypes>::Type ... arguments)
    {
        JNIEnv* env = getCurrentJNIEnvironment();

        std::string methodSignature = getJavaMethodSignature<void, ArgumentTypes...>();

        jclass javaClass = env->FindClass(className.c_str());
        if (javaClass == nullptr) {
            reportInternalError("class not found [" + className + "]");
            return nullptr;
        }

        jmethodID javaConstructor = env->GetMethodID(javaClass, "<init>", methodSignature.c_str());
        if (javaConstructor == nullptr) {
            reportInternalError("constructor for class [" + className + "] not found, tried signature [" + methodSignature + "]");
            return nullptr;
        }

        jobject result = env->NewObject(javaClass, javaConstructor, arguments...);

        env->DeleteLocalRef(javaClass);

        return result;
    }

    /**
    * New style of java object creation, where object class is specified
    * in the template arguments.
    *
    * @param arguments List of arguments for the constructor.
    * @return Create Java object pointer (aka jobject).
    */
    template<class NewObjectType, class ... ArgumentTypes>
    jobject createNewObject(typename ToJavaType<ArgumentTypes>::Type ... arguments)
    {
        return createNewObject<ArgumentTypes...>(NewObjectType::className(), arguments...);
    }
}

#endif
