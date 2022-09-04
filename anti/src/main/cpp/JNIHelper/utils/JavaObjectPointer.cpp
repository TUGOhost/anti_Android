/**
    \file JavaObjectPointer.cpp
    \brief RAII class to store jobject reference as a global reference.
    \author Denis Sorokin
    \date 28.01.2016
*/

#include "../core/JNIEnvironment.hpp"
#include "../utils/JavaObjectPointer.hpp"

namespace jh
{
    JavaObjectPointer::JavaObjectPointer()
    : m_jobjectGlobalReference(nullptr)
    {
        // nothing to do here
    }

    JavaObjectPointer::JavaObjectPointer(jobject object)
    : m_jobjectGlobalReference(nullptr)
    {
        reset(object);
    }

    JavaObjectPointer::JavaObjectPointer(const JavaObjectPointer& other)
    : m_jobjectGlobalReference(nullptr)
    {
        reset(other);
    }

    JavaObjectPointer::~JavaObjectPointer()
    {
        release();
    }

    JavaObjectPointer& JavaObjectPointer::operator=(const JavaObjectPointer& other)
    {
        if (&other == this)
            return *this;

        reset(other.get());

        return *this;
    }

    JavaObjectPointer& JavaObjectPointer::operator=(JavaObjectPointer&& other)
    {
        if (&other == this)
            return *this;

        reset(other.get());
        other.release();

        return *this;
    }

    JavaObjectPointer& JavaObjectPointer::operator=(const jobject other)
    {
        reset(other);

        return *this;
    }

    void JavaObjectPointer::reset(jobject object)
    {
        JNIEnv* env = getCurrentJNIEnvironment();

        if (object) {
            object = env->NewGlobalRef(object);
        }

        if (m_jobjectGlobalReference) {
            env->DeleteGlobalRef(m_jobjectGlobalReference);
        }

        m_jobjectGlobalReference = object;
    }

    void JavaObjectPointer::release()
    {
        reset(nullptr);
    }

    jobject JavaObjectPointer::get() const
    {
        return m_jobjectGlobalReference;
    }

    JavaObjectPointer::operator jobject() const
    {
        return get();
    }

    JavaObjectPointer::operator bool() const
    {
        return !areEqual(get(), nullptr);
    }
}
