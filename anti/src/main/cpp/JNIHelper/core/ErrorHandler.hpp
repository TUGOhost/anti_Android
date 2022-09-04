/**
    \file ErrorHandler.hpp
    \brief Describes how internal errors should be reported.
    \author Denis Sorokin
    \date 24.01.2016
*/

#ifndef JH_ERROR_HANDLER_HPP
#define JH_ERROR_HANDLER_HPP

#include <string>
#include <android/log.h>

namespace jh
{
    /**
    * Reports some error that happened during JNI-related actions to user.
    * This method is intended to be used only by this library.
    *
    * @param errorMessage Message string describing the error.
    */
    inline void reportInternalError(std::string errorMessage)
    {
        __android_log_write(ANDROID_LOG_ERROR, "ZFJavaHelper", ("JavaHelper internal error: '" + errorMessage + "'.").c_str());
    }

    /**
    * Reports some information to user.
    * This method is intended to be used only by this library.
    *
    * @param message Message string that should be reported to user.
    */
    inline void reportInternalInfo(std::string message)
    {
        __android_log_write(ANDROID_LOG_INFO, "ZFJavaHelper", ("JavaHelper message: '" + message + "'.").c_str());
    }
}

#endif
