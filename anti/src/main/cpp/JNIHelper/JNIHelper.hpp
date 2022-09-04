/**
    \file JNIHelper.hpp
    \brief A small utility library to hide all the horrors of JNI API.
    \author Denis Sorokin
    \date 24.01.2016
*/

/**
* Changelog:
*
* ===> Version 1.1.0:
* > Java array support (arrays like 'int[]')
* > Static native methods registration
* > LocalReferenceFrame - class to automatically free local references
* > Java string utility function (transformations from and to std::string)
* > Removed 'zframework/debug.h' dependency
* > Removed 'this' pointer from 'JavaObjectWrapper' class constructor
* > Reworked custom java classes internal representation
* > Reworked type deduction system
* > Reworked file structure
* > Renamed to 'JNIHelper'
*
* ===> Version 1.0.1:
* > JavaObjectWrapper class
*
* ===> Version 1.0.0:
* > Basic functionality to call java methods from C++
*/

#ifndef JH_JAVA_HELPER_HPP
#define JH_JAVA_HELPER_HPP

/**
* ==================== JAVA ARRAYS ====================
* @code{.cpp}
*
* // Define custom java class:
* JH_JAVA_CUSTOM_CLASS(JavaExample, "com/some/path/Example");
*
* // Create int array:
* std::vector<int> v({10, 15, 20});
* jintArray intArray = jh::JavaArrayBuilder<int>()
*     .add(5)                   // value
*     .add({25, 30})            // initializer_list
*     .add(v.begin(), v.end())  // iterators
*     .build());
*
* // Parse java int array:
* auto stdIntVector = jh::jarrayToVector<jintArray>(intArray);
* for (auto i : stdIntVector) {
*     cout << i << endl;
* }
*
* // Passing java array to java code:
* jh::callMethod<void, jh::JavaArray<JavaExample>>(exampleObject, "someMethod", exampleArray);
*
* // Getting java array from java code:
* jobjectArray someArray = jh::callStaticMethod<JavaExample, JavaArray<jstring>>("otherMethod");
*
* @endcode
*/
#include "arrays/ArrayBuilder.hpp"

/**
* ==================== STATIC CALLS ====================
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
#include "calls/StaticCaller.hpp"

/**
* ==================== INSTANCE CALLS ====================
* @code{.cpp}
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
#include "calls/InstanceCaller.hpp"

/**
* ==================== OBJECT CREATION ====================
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
#include "calls/ObjectCreation.hpp"

/**
* ==================== JAVA CUSTOM CLASSES ====================
* @code{.cpp}
*
* // Declare custom Java class (named Example):
* JH_JAVA_CUSTOM_CLASS(Example, "com/class/path/Example");
*
* // Get the Java custom class name:
* std::string className = Example::className();
*
* @endcode
*/
#include "core/JavaCustomClass.hpp"

/**
* ==================== JNI ENVIRONMENT ====================
* @code{.cpp}
*
* // Lets assume that we have entered some scope and we are in a non-java thread:
* {
*
*     // Attaching this thread to JVM:
*     jh::JavaEnvironmentGuarantee javaContext();
*
*     // Get JNIEnv* pointer and do something with it:
*     auto env = jh::getCurrentJNIEnvironment();
*
*     // Doing something with Java code; for example, creating jstring...
*     ...
*
* // Leaving the scope; this thread will be detached from JVM:
* }
*
* @endcode
*/
#include "core/JNIEnvironment.hpp"

/**
* ==================== JAVA OBJECT WRAPPER ====================
* @code{.java}
*
* // Let's assume that we have some java class like this:
* package com.some.path;
*
* public class Example
* {
*     ...
*     protected native void someVoidMethod();
*     protected native int sumTwo(int x, int y);
*     ...
* }
*
* @endcode
*
* @code{.cpp}
*
* // Let's declare some java class:
* JH_JAVA_CUSTOM_CLASS(JavaExample, "com/some/path/Example");
*
* // Now declare our wrapper:
* class ExampleWrapper : public jh::JavaObjectWrapper<JavaExample, ExampleWrapper>
* {
*     // We need to implement two abstract methods - 'linkJavaNativeMethods' and 'initializeJavaObject'.
*
*     // Inside 'linkJavaNativeMethods' method we should register all native methods of the wrapped class.
*     // First argument should be unique method ID for this wrapper class:
*     void linkJavaNativeMethods() override
*     {
*         registerNativeMethod<1, void>("someVoidMethod", &ExampleWrapper::interstitialPressed);
*         registerNativeMethod<2, int, int, int>("sumTwo", &ExampleWrapper::interstitialClosed);
*     }
*
*     // Inside 'initializeJavaObject' method we should created and return an instance of wrapped java class.
*     // This method will never be called after it returns a non-nullptr value. This method also supports lazy
*     // initialization; if it returns nullptr, then it will be called again after the next 'object()' method call.
*     jobject initializeJavaObject() override
*     {
*         return jh::createNewObject<ExampleClass>();
*     }
*
*     // Now we should implement those native methods:
*     void someVoidMethod() { log("Example", "Look, I'm logging something!"); }
*     int sumTwo(int x, int y) { return x + y; }
*
*     // In case we use some lazy initialization:
*     void doSomething()
*     {
*         if (auto wrapper = object()) {
*             // We can use wrapper object.
*         } else {
*             // Do something else.
*         }
*     }
* }
* @endcode
*/
#include "native/JavaObjectWrapper.hpp"

/**
* ==================== JAVA NATIVE METHODS ====================
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
#include "native/JavaNativeMethod.hpp"

/**
* ==================== LOCAL REFERENCE FRAME ====================
* @code{.cpp}
*
* // String s1 is created BEFORE the local frame creation:
* jstring s1 = jh::createJString("s1");
*
* // Local frame creation:
* jh::LocalReferenceFrame frame;
*
* // String s2 is created AFTER the local frame creation:
* jstring s2 = jh::createJString("s2");
*
* // Both strings are valid now; lets pop the frame:
* frame.pop();
*
* // String s1 is fine now, but string s2 is dead; you shouldn't use it!
* // Lets recreate the frame and string s2:
* frame.push();
* s2 = jh::createJString("s2");
*
* // Both strings are again alive.
* // Now lets pop the frame, but leave the string s2 alive:
* frame.pop(&s2);
*
* // Both strings are still alive!
* log(jh::jstringToStdString(s1) + jh::jstringToStdString(s2));
*
* @endcode
*/
#include "utils/LocalReferenceFrame.hpp"

/**
* ==================== JAVA OBJECT POINTER ====================
* @code{.cpp}
*
* // Declare two Java object pointers:
* jh::JavaObjectPointer obj;
* jh::JavaObjectPointer obj2;
*
* // Declare some special Java class:
* JH_JAVA_CUSTOM_CLASS(Example, "com/class/path/Example");
*
* // Can check if pointer holds any jobject:
* if (!obj)
*     log("pointer is empty!");
*
* // Can be assigned from jobject:
* obj = jh::createNewObject<Example>();
*
* // Can be assigned from other JavaObjectPointer:
* obj2 = obj;
*
* // Can be compared:
* if (obj == obj2)
*     log("they are the same");
*
* // Can be used as an argument in Java calls:
* jh::callStaticMethod<Example, void, Example>("someMethodName", obj);
*
* @endcode
*/
#include "utils/JavaObjectPointer.hpp"

/**
* ==================== JAVA STRING ====================
* @code{.cpp}
*
* // Creating java string:
* jstring js = jh::createJString("someText");
*
* // Transforming java string to std::string:
* std::string ss = jh::jstringToStdString(js);
*
* @endcode
*/
#include "utils/JStringUtils.hpp"

#endif
