/**
    \file ArrayGetter.hpp
    \brief Internal implementation of getting elements from java arrays.
    \author Denis Sorokin
    \date 22.02.2016
*/

#ifndef JH_ARRAY_GETTER_HPP
#define JH_ARRAY_GETTER_HPP

#include <vector>
#include <jni.h>

namespace jh
{
    /**
    * Stub implementations for getting elements from java arrays.
    */
    template<class JavaArrayType>
    struct JavaArrayGetter;

    /**
    * Implementations for getting elements from java boolean arrays.
    */
    template<>
    struct JavaArrayGetter<jbooleanArray>
    {
        static std::vector<jboolean> get(JNIEnv* env, jbooleanArray array)
        {
            jint size = env->GetArrayLength(array);
            jboolean* elements = env->GetBooleanArrayElements(array, nullptr);

            std::vector<jboolean> result(static_cast<std::size_t>(size));
            for (int i = 0; i < size; ++i) {
                result[i] = elements[i];
            }

            env->ReleaseBooleanArrayElements(array, elements, JNI_ABORT);

            return std::move(result);
        }
    };

    /**
    * Implementations for getting elements from java int arrays.
    */
    template<>
    struct JavaArrayGetter<jintArray>
    {
        static std::vector<jint> get(JNIEnv* env, jintArray array)
        {
            jint size = env->GetArrayLength(array);
            jint* elements = env->GetIntArrayElements(array, nullptr);

            std::vector<jint> result(static_cast<std::size_t>(size));
            for (int i = 0; i < size; ++i) {
                result[i] = elements[i];
            }

            env->ReleaseIntArrayElements(array, elements, JNI_ABORT);

            return std::move(result);
        }
    };

    /**
    * Implementations for getting elements from java long arrays.
    */
    template<>
    struct JavaArrayGetter<jlongArray>
    {
        static std::vector<jlong> get(JNIEnv* env, jlongArray array)
        {
            jint size = env->GetArrayLength(array);
            jlong* elements = env->GetLongArrayElements(array, nullptr);

            std::vector<jlong> result(static_cast<std::size_t>(size));
            for (int i = 0; i < size; ++i) {
                result[i] = elements[i];
            }

            env->ReleaseLongArrayElements(array, elements, JNI_ABORT);

            return std::move(result);
        }
    };

    /**
    * Implementations for getting elements from java float arrays.
    */
    template<>
    struct JavaArrayGetter<jfloatArray>
    {
        static std::vector<jfloat> get(JNIEnv* env, jfloatArray array)
        {
            jint size = env->GetArrayLength(array);
            jfloat* elements = env->GetFloatArrayElements(array, nullptr);

            std::vector<jfloat> result(static_cast<std::size_t>(size));
            for (int i = 0; i < size; ++i) {
                result[i] = elements[i];
            }

            env->ReleaseFloatArrayElements(array, elements, JNI_ABORT);

            return std::move(result);
        }
    };

    /**
    * Implementations for getting elements from java double arrays.
    */
    template<>
    struct JavaArrayGetter<jdoubleArray>
    {
        static std::vector<jdouble> get(JNIEnv* env, jdoubleArray array)
        {
            jint size = env->GetArrayLength(array);
            jdouble* elements = env->GetDoubleArrayElements(array, nullptr);

            std::vector<jdouble> result(static_cast<std::size_t>(size));
            for (int i = 0; i < size; ++i) {
                result[i] = elements[i];
            }

            env->ReleaseDoubleArrayElements(array, elements, JNI_ABORT);

            return std::move(result);
        }
    };

    /**
    * Implementations for getting elements from java object arrays.
    */
    template<>
    struct JavaArrayGetter<jobjectArray>
    {
        static std::vector<jobject> get(JNIEnv* env, jobjectArray array)
        {
            jint size = env->GetArrayLength(array);

            std::vector<jobject> result(static_cast<std::size_t>(size));
            for (int i = 0; i < size; ++i) {
                result[i] = env->GetObjectArrayElement(array, i);
            }

            return std::move(result);
        }
    };
}

#endif
