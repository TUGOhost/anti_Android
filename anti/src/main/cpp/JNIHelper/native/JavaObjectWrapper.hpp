/**
    \file JavaObjectWrapper.hpp
    \brief C++ wrapper of some java class.
    \author Denis Sorokin
    \date 15.01.2016
*/

/*
* Cheat sheet:
*
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
*
* @endcode
*/

#ifndef JH_JAVA_OBJECT_WRAPPER_HPP
#define JH_JAVA_OBJECT_WRAPPER_HPP

#include <set>
#include <vector>
#include <functional>
#include <jni.h>
#include "../core/ToJavaType.hpp"
#include "../core/ErrorHandler.hpp"
#include "../native/JavaNativeMethod.hpp"
#include "../utils/JavaObjectPointer.hpp"

namespace jh
{
    /**
    * Small utility structure to hold information about java native methods.
    *
    * @param name Name of the java native method.
    * @param signature Full signature of the java native method.
    * @param pointer A pointer to the C++ function that should be called as native method.
    */
    struct NativeMethodDescription
    {
        std::string name;
        std::string signature;
        void* pointer;
    };

    /**
    * Class that wraps some java class by the C++ class.
    * It was designed that the java class instances should be created only by this wrapper class.
    *
    * @param InternalJavaClass The java class that should be wrapped. Should be declared by JH_JAVA_CUSTOM_CLASS macro.
    * @param WrapperClass The C++ class that wraps the java class. Should be equal to the class that derives from JavaObjectWrapper.
    */
    template <class InternalJavaClass, class WrapperClass>
    class JavaObjectWrapper
    {
    public:
        /**
        * Declaring types for later usage by internal code.
        */
        using JavaClass = InternalJavaClass;
        using CppClass = WrapperClass;

    private:
        /**
        * Information about exsiting links between java objects and cpp wrappers.
        */
        using WrapperObjectInfo = std::pair<jobject, CppClass*>;

        /**
        * Native methods should have full access to the wrapper class.
        */
        template<int, class, class, class ...>
        friend class JavaNativeMethod;

        /**
        * Pool of all wrapper objects and their jobjects.
        */
        static std::set<WrapperObjectInfo> s_objectsCollection;

        /**
        * Finds the wrapper object for jobject instance and calls the required method.
        */
        template<class MethodReturnType>
        static MethodReturnType callCppObjectMethod(jobject javaObject, std::function<MethodReturnType(CppClass*)> callback)
        {
            for (auto& entry: s_objectsCollection) {
                if (jh::areEqual(entry.first, javaObject)) {
                    return callback(entry.second);
                }
            }

            reportInternalError("couldn't call java native method - cpp object not found!");

            return MethodReturnType();
        }

        /**
        * Registers the pair of wrapper object and the jobject.
        */
        void registerObject(jobject javaObject, CppClass* cppObject)
        {
            s_objectsCollection.insert(WrapperObjectInfo(javaObject, cppObject));
        }

        /**
        * Unregisters the pair of wrapper object and the jobject.
        */
        void unregisterObject(jobject javaObject, CppClass* cppObject)
        {
            auto entry = s_objectsCollection.find(WrapperObjectInfo(javaObject, cppObject));

            if (entry != s_objectsCollection.end()) {
                s_objectsCollection.erase(entry);
            } else {
                reportInternalError("unable to unregister cpp object - not found");
            }
        }

    public:
        /**
        * Default constructor for java wrapper object.
        */
        JavaObjectWrapper()
        : m_javaObject(nullptr)
        {
            // nothing to do here
        }

        /**
        * Removes this wrapper calss from the global wrapper's pool.
        */
        virtual ~JavaObjectWrapper()
        {
            unregisterObject(m_javaObject, static_cast<CppClass*>(this));
        }

        /**
        * Returns the jobject of this wrapper class instance. Will also link
        * native methods and initialize the internal java object if it wasn't done before.
        */
        jobject object()
        {
            if (!s_nativeMethodsWereRegistered) {
                linkJavaNativeMethods();

                std::vector<JNINativeMethod> descriptions;
                for (auto& description : s_nativeMethodsDescriptions) {
                    descriptions.push_back({
                        description.name.c_str(),
                        description.signature.c_str(),
                        description.pointer
                    });
                }

                if (s_nativeMethodsDescriptions.size() > 0) {
                    s_nativeMethodsWereRegistered = registerJavaNativeMethods(
                            InternalJavaClass::className(),
                            descriptions.size(),
                            &descriptions[0]
                    );
                } else {
                    s_nativeMethodsWereRegistered = true;
                }
            }

            if (!m_javaObject) {
                m_javaObject = initializeJavaObject();

                if (m_javaObject) {
                    registerObject(m_javaObject, static_cast<CppClass*>(this));
                }
            }

            return m_javaObject;
        }

    private:
        /**
        * Pointer to the wrapped java object.
        */
        JavaObjectPointer m_javaObject;

        /**
        * This method should return the java object instance that is wrapped by this class.
        */
        virtual jobject initializeJavaObject() = 0;

        /**
        * Holds the information about native methods registration.
        */
        static bool s_nativeMethodsWereRegistered;
        static std::vector<NativeMethodDescription> s_nativeMethodsDescriptions;

        /**
        * This method should specify all necessary native methods that will be used by the java class.
        */
        virtual void linkJavaNativeMethods() = 0;

    protected:
        /**
        * Registers the local method as a native java method
        *
        * @param id Dirty hack to distinguish two native methods with equal signatures. Should be unique for every native method per wrapper class.
        * @param ReturnType The return type of java native method.
        * @param Arguments The types of arguments of java native method.
        *
        * @warning Each combination of template parameters should correspond to ONLY ONE native method.
        * @warning Native methods that were registered this way should NOT be called inside the java object constructor.
        */
        template<int id, class ReturnType, class ... Arguments>
        void registerNativeMethod(std::string methodName, typename ToJavaType<ReturnType>::Type (CppClass::*methodPointer)(typename ToJavaType<Arguments>::Type...))
        {
            using NativeMethodClass = JavaNativeMethod<id, CppClass, typename ToJavaType<ReturnType>::Type, typename ToJavaType<Arguments>::Type ...>;

            s_nativeMethodsDescriptions.push_back({
                methodName,
                getJavaMethodSignature<ReturnType, Arguments...>(),
                (void*)&NativeMethodClass::rawNativeMethod
            });

            NativeMethodClass::setCallback(methodPointer);
        }
    };

    template <class InternalJavaClass, class WrapperClass>
    std::set<typename JavaObjectWrapper<InternalJavaClass, WrapperClass>::WrapperObjectInfo> JavaObjectWrapper<InternalJavaClass, WrapperClass>::s_objectsCollection;

    template <class InternalJavaClass, class WrapperClass>
    std::vector<NativeMethodDescription> JavaObjectWrapper<InternalJavaClass, WrapperClass>::s_nativeMethodsDescriptions;

    template <class InternalJavaClass, class WrapperClass>
    bool JavaObjectWrapper<InternalJavaClass, WrapperClass>::s_nativeMethodsWereRegistered = false;
}

#endif
