/**
    \file ArraySetter.hpp
    \brief Internal implementation of setting elements inside java arrays.
    \author Denis Sorokin
    \date 22.02.2016
*/

#ifndef JH_ARRAY_SETTER_HPP
#define JH_ARRAY_SETTER_HPP

#include <jni.h>

namespace jh
{
    /**
    * Stub implementations for setting elements of java arrays.
    */
    template<class JavaArrayType>
    struct JavaArraySetter;

    /**
    * Implementations for setting elements of java boolean arrays.
    */
    template<>
    struct JavaArraySetter<jbooleanArray>
    {
        static void set(JNIEnv* env, jbooleanArray array, jsize size, jboolean* elements)
        {
            env->SetBooleanArrayRegion(array, 0, size, elements);
        }
    };

    /**
    * Implementations for setting elements of java int arrays.
    */
    template<>
    struct JavaArraySetter<jintArray>
    {
        static void set(JNIEnv* env, jintArray array, jsize size, jint* elements)
        {
            env->SetIntArrayRegion(array, 0, size, elements);
        }
    };

    /**
    * Implementations for setting elements of java long arrays.
    */
    template<>
    struct JavaArraySetter<jlongArray>
    {
        static void set(JNIEnv* env, jlongArray array, jsize size, jlong* elements)
        {
            env->SetLongArrayRegion(array, 0, size, elements);
        }
    };

    /**
    * Implementations for setting elements of java float arrays.
    */
    template<>
    struct JavaArraySetter<jfloatArray>
    {
        static void set(JNIEnv* env, jfloatArray array, jsize size, jfloat* elements)
        {
            env->SetFloatArrayRegion(array, 0, size, elements);
        }
    };

    /**
    * Implementations for setting elements of java double arrays.
    */
    template<>
    struct JavaArraySetter<jdoubleArray>
    {
        static void set(JNIEnv* env, jdoubleArray array, jsize size, jdouble* elements)
        {
            env->SetDoubleArrayRegion(array, 0, size, elements);
        }
    };

    /**
    * Implementations for setting elements of java object arrays.
    */
    template<>
    struct JavaArraySetter<jobjectArray>
    {
        static void set(JNIEnv* env, jobjectArray array, jsize size, jobject* elements)
        {
            for (int i = 0; i < size; ++i) {
                env->SetObjectArrayElement(array, i, elements[i]);
            }
        }
    };
}

#endif
