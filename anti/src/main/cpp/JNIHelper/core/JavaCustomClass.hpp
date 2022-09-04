/**
    \file JavaCustomClass.hpp
    \brief Allows user to declare custom java classes and deduce method signatures for them.
    \author Denis Sorokin
    \date 24.01.2016
*/

/**
* Cheat sheet:
*
* @code{.cpp}
*
* // Declare custom Java class (named Example):
* JH_JAVA_CUSTOM_CLASS(Example, "com/class/path/Example");
*
* // Get the Java custom class name:
* std::string className = Example::className();
*
* // Call some static method of this class:
* int result = jh::callStaticMethod<Example, int>("methodName");
*
* // Explicitly create the object of this class:
* jobject exampleObject = jh::createNewObject<Example>();
*
* // Call some method that returns the object of this class:
* jobject exampleObject2 = jh::callStaticMethod<Example, Example>("methodName");
*
* // Pass the object of this class as a parameter:
* jh::callMethod<void, Example>(someOtherObject, "methodName", exampleObject);
*
* @endcode
*/

#ifndef JH_JAVA_CUSTOM_CLASS_HPP
#define JH_JAVA_CUSTOM_CLASS_HPP

#include <jni.h>
#include <string>

/**
* This macro magic tells the library about the existance of some android class.
* This allows to perform java method signature deduction and some other stuff to happen.
* After declaring this macro programmer can use the declared class in the JNI calls.
*
* All methods that should return the instance of custom Java class would return
* the pointer to Java object (aka jobject). The programmer should carefully track
* the types of Java objects pointers by himself.
*/
#define JH_JAVA_CUSTOM_CLASS(CLASS_NAME_TOKEN, CLASS_PATH_STRING)               \
struct CLASS_NAME_TOKEN                                                         \
{                                                                               \
    static std::string className()                                              \
    {                                                                           \
        return CLASS_PATH_STRING;                                               \
    }                                                                           \
    static std::string signature()                                              \
    {                                                                           \
        return "L" CLASS_PATH_STRING ";";                                       \
    }                                                                           \
};

#endif
