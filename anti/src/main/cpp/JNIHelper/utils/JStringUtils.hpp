/**
    \file JStringUtils.hpp
    \brief Utility functions to use jstring objects.
    \author Denis Sorokin
    \date 24.01.2016
*/

/**
* Cheat sheet:
*
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

#ifndef JH_JSTRING_UTILS_HPP
#define JH_JSTRING_UTILS_HPP

#include <jni.h>
#include <string>

namespace jh
{
    /**
    * Creates a new java string from C-style string.
    *
    * @param str String to be converted.
    * @return Equivalent java string.
    */
    jstring createJString(const char* str);

    /**
    * Creates a new java string from C++-style string.
    *
    * @param str String to be converted.
    * @return Equivalent java string.
    */
    jstring createJString(const std::string str);

    /**
    * Creates a std::string from a java string pointer.
    *
    * @param str Java string pointer.
    * @return Equivalent std::string string.
    */
    std::string jstringToStdString(const jstring str);
}

#endif
