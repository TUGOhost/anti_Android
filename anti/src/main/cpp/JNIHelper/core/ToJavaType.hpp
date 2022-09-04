/**
    \file ToJavaType.hpp
    \brief Internal type deduction.
    \author Denis Sorokin
    \date 21.02.2016
*/

#ifndef JH_TO_JAVA_TYPE_HPP
#define JH_TO_JAVA_TYPE_HPP

#include <string>
#include <jni.h>

namespace jh
{
    /**
    * Structure that provides some information about internal types.
    *
    * @param Type The corresponding JNI type of the 'T' object.
    * @param Type The corresponding JNI type of the 'T' object which is returned by JNI calls.
    * @param signature The signature of 'T' type.
    * @param className The class name (path?) of 'T' type, if it is some default/custom java type.
    */
    template<class T>
    struct ToJavaType
    {
        using Type = jobject;
        using CallReturnType = jobject;

        static std::string signature()
        {
            return T::signature();
        }

        static std::string className()
        {
            return T::className();
        }
    };

    /**
    * Structure that provides some information about void type.
    */
    template<>
    struct ToJavaType<void>
    {
        using Type = void;
        using CallReturnType = void;

        static std::string signature()
        {
            return "V";
        }
    };

    /**
    * Structure that provides some information about bool type.
    */
    template<>
    struct ToJavaType<bool>
    {
        using Type = jboolean;
        using CallReturnType = jboolean;

        static std::string signature()
        {
            return "Z";
        }
    };

    /**
    * Structure that provides some information about int type.
    */
    template<>
    struct ToJavaType<int>
    {
        using Type = jint;
        using CallReturnType = jint;

        static std::string signature()
        {
            return "I";
        }
    };

    /**
    * Structure that provides some information about long type.
    */
    template<>
    struct ToJavaType<long>
    {
        using Type = jlong;
        using CallReturnType = jlong;

        static std::string signature()
        {
            return "J";
        }
    };

    /**
    * Structure that provides some information about float type.
    */
    template<>
    struct ToJavaType<float>
    {
        using Type = jfloat;
        using CallReturnType = jfloat;

        static std::string signature()
        {
            return "F";
        }
    };

    /**
    * Structure that provides some information about double type.
    */
    template<>
    struct ToJavaType<double>
    {
        using Type = jdouble;
        using CallReturnType = jdouble;

        static std::string signature()
        {
            return "D";
        }
    };

    /**
    * Structure that describes a type that can be casted to jobject.
    */
    template<class JavaType>
    struct JPointerLike
    {
        using Type = JavaType;
        using CallReturnType = jobject;
    };

    /**
    * Structure that provides some information about jobject type.
    */
    template<>
    struct ToJavaType<jobject> : public JPointerLike<jobject>
    {
        static std::string className()
        {
            return "java/lang/Object";
        }

        static std::string signature()
        {
            return "L" + className() + ";";
        }
    };

    /**
    * Structure that provides some information about jstring type.
    */
    template<>
    struct ToJavaType<jstring> : public JPointerLike<jstring>
    {
        static std::string className()
        {
            return "java/lang/String";
        }

        static std::string signature()
        {
            return "L" + className() + ";";
        }
    };

    /**
    * Structure that describes the types of java arrays.
    *
    * @param ElementType The corresponding JNI type of the elements inside this array.
    */
    template<class JavaElementType>
    struct JavaArray
    {
        using ElementType = JavaElementType;

        static std::string signature()
        {
            return "[" + ToJavaType<JavaElementType>::signature();
        }
    };

    /**
    * Structure that describes the types of custom java arrays.
    */
    template<>
    template<class JavaType>
    struct ToJavaType<JavaArray<JavaType>> : public JavaArray<JavaType>
    {
        using Type = jobjectArray;
        using CallReturnType = jobjectArray;
    };

    /**
    * Structure that provides some information about jbooleanArray type.
    */
    template<>
    struct ToJavaType<jbooleanArray> : public JavaArray<jboolean>, public JPointerLike<jbooleanArray>
    { };

    /**
    * Structure that provides some information about jintArray type.
    */
    template<>
    struct ToJavaType<jintArray> : public JavaArray<jint>, public JPointerLike<jintArray>
    { };

    /**
    * Structure that provides some information about jlongArray type.
    */
    template<>
    struct ToJavaType<jlongArray> : public JavaArray<jlong>, public JPointerLike<jlongArray>
    { };

    /**
    * Structure that provides some information about jfloatArray type.
    */
    template<>
    struct ToJavaType<jfloatArray> : public JavaArray<jfloat>, public JPointerLike<jfloatArray>
    { };

    /**
    * Structure that provides some information about jdoubleArray type.
    */
    template<>
    struct ToJavaType<jdoubleArray> : public JavaArray<jdouble>, public JPointerLike<jdoubleArray>
    { };

    /**
    * Structure that provides some information about jobjectArray type.
    */
    template<>
    struct ToJavaType<jobjectArray> : public JavaArray<jobject>, public JPointerLike<jobjectArray>
    { };
}

#endif
