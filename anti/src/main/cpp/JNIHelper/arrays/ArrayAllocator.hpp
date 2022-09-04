/**
    \file ArrayAllocator.hpp
    \brief Internal implementations to create different java arrays.
    \author Denis Sorokin
    \date 22.02.2016
*/

#ifndef JH_ARRAY_ALLOCATOR_HPP
#define JH_ARRAY_ALLOCATOR_HPP

#include <jni.h>
#include "../core/ErrorHandler.hpp"

namespace jh
{
    /**
    * Stub implementation of java array creation.
    */
    template<class JavaArrayType, class ElementType>
    struct JavaArrayAllocator;

    /**
    * Implementation of java boolean array creation.
    */
    template<class ElementType>
    struct JavaArrayAllocator<jbooleanArray, ElementType>
    {
        static jbooleanArray create(JNIEnv* env, jsize size)
        {
            return env->NewBooleanArray(size);
        }
    };

    /**
    * Implementation of java int array creation.
    */
    template<class ElementType>
    struct JavaArrayAllocator<jintArray, ElementType>
    {
        static jintArray create(JNIEnv* env, jsize size)
        {
            return env->NewIntArray(size);
        }
    };

    /**
    * Implementation of java long array creation.
    */
    template<class ElementType>
    struct JavaArrayAllocator<jlongArray, ElementType>
    {
        static jlongArray create(JNIEnv* env, jsize size)
        {
            return env->NewLongArray(size);
        }
    };

    /**
    * Implementation of java float array creation.
    */
    template<class ElementType>
    struct JavaArrayAllocator<jfloatArray, ElementType>
    {
        static jfloatArray create(JNIEnv* env, jsize size)
        {
            return env->NewFloatArray(size);
        }
    };

    /**
    * Implementation of java double array creation.
    */
    template<class ElementType>
    struct JavaArrayAllocator<jdoubleArray, ElementType>
    {
        static jdoubleArray create(JNIEnv* env, jsize size)
        {
            return env->NewDoubleArray(size);
        }
    };

    /**
    * Implementation of java object array creation.
    *
    * @param ElementType Type of java object in the array.
    */
    template<class ElementType>
    struct JavaArrayAllocator<jobjectArray, ElementType>
    {
        static jobjectArray create(JNIEnv* env, jsize size)
        {
            std::string className = ToJavaType<ElementType>::className();

            jclass javaClass = env->FindClass(className.c_str());
            if (javaClass == nullptr) {
                reportInternalError("class not found [" + className + "]");
                return nullptr;
            }

            return env->NewObjectArray(size, javaClass, nullptr);
        }
    };
}

#endif
