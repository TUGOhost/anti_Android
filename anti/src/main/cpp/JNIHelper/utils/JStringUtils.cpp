/**
    \file JStringUtils.cpp
    \brief Utility functions to use jstring objects.
    \author Denis Sorokin
    \date 24.01.2016
*/

#include "../core/JNIEnvironment.hpp"
#include "../utils/JStringUtils.hpp"

namespace jh
{
    jstring createJString(const char* str)
    {
        JNIEnv* env = getCurrentJNIEnvironment();
        return env->NewStringUTF(str);
    }

    jstring createJString(const std::string str)
    {
        JNIEnv* env = getCurrentJNIEnvironment();
        return env->NewStringUTF(str.c_str());
    }

    jstring createJString(const std::string& str)
    {
        JNIEnv* env = getCurrentJNIEnvironment();
        return env->NewStringUTF(str.c_str());
    }

    std::string jstringToStdString(const jstring javaString)
    {
        JNIEnv* env = getCurrentJNIEnvironment();

        const char* nativeString = env->GetStringUTFChars(javaString, nullptr);
        std::string str(nativeString);
        env->ReleaseStringUTFChars(javaString, nativeString);

        return str;
    }
}
