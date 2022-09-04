/**
    \file JavaObjectPointer.hpp
    \brief RAII class to store jobject reference as a global reference.
    \author Denis Sorokin
    \date 28.01.2016
*/

/**
* Cheat sheet:
*
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
*     log("they are the same object");
*
* // Can be used as an argument in Java calls:
* jh::callStaticMethod<Example, void, Example>("someMethodName", obj);
*
* @endcode
*/

#ifndef JH_JAVA_OBJECT_POINTER_HPP
#define JH_JAVA_OBJECT_POINTER_HPP

#include <jni.h>
#include "../core/JNIEnvironment.hpp"

namespace jh
{
    /**
    * Java object (aka jobject) RAII wrapper that holds jobject as a global Java
    * reference. Can be assigned from jobject, casted to jobject and so on.
    */
    class JavaObjectPointer
    {
    public:
        JavaObjectPointer();
        JavaObjectPointer(jobject object);
        JavaObjectPointer(const JavaObjectPointer& other);
        ~JavaObjectPointer();

        JavaObjectPointer& operator=(const jobject other);
        JavaObjectPointer& operator=(const JavaObjectPointer& other);
        JavaObjectPointer& operator=(JavaObjectPointer&& other);

        void release();
        void reset(jobject object);
        jobject get() const;

        operator jobject() const;
        operator bool() const;

    private:
        jobject m_jobjectGlobalReference;
    };

    /**
    * Checks if two Java objects (aka jobject) are the same object.
    *
    * @param lhs One object.
    * @param rhs Another object.
    * @return True if they are the same object, or false otherwise.
    */
    inline bool areEqual(const jobject lhs, const jobject rhs)
    {
        JNIEnv* env = getCurrentJNIEnvironment();
        return env->IsSameObject(lhs, rhs);
    }

    /**
    * Checks if two Java object wrappers point to the same object.
    *
    * @param lhs One Java object wrapper.
    * @param rhs Another Java object wrapper.
    * @return True if both wrappers point to the same object, or false otherwise.
    */
    inline bool operator==(const JavaObjectPointer& lhs, const JavaObjectPointer& rhs)
    {
        return areEqual(lhs, rhs);
    }

    /**
    * Checks if Java object wrapper points to the other Java object.
    *
    * @param lhs Java object (aka jobject).
    * @param rhs Java object wrapper.
    * @return True if Java object wrapper points to the other Java object, or false otherwise.
    */
    inline bool operator==(const jobject lhs, const JavaObjectPointer& rhs)
    {
        return areEqual(lhs, rhs);
    }

    /**
    * Checks if Java object wrapper points to the other Java object.
    *
    * @param lhs Java object wrapper.
    * @param rhs Java object (aka jobject).
    * @return True if Java object wrapper points to the other Java object, or false otherwise.
    */
    inline bool operator==(const JavaObjectPointer& lhs, const jobject rhs)
    {
        return areEqual(lhs, rhs);
    }
}

#endif
