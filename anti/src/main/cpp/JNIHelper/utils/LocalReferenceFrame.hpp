/**
   \file LocalReferenceFrame.hpp
   \brief Class to clear local java references.
   \author Denis Sorokin
   \date 21.02.2016
*/

/**
* Cheat sheet:
*
* @code{.cpp}
*
* // String s1 is created BEFORE the local frame creation:
* jstring s1 = jh::createJString("s1");
*
* // Local frame creation:
* jh::LocalReferenceFrame frame;
*
* // String s2 is created AFTER the local frame creation:
* jstring s2 = jh::createJString("s2");
*
* // Both strings are valid now; lets pop the frame:
* frame.pop();
*
* // String s1 is fine now, but string s2 is dead; you shouldn't use it!
* // Lets recreate the frame and string s2:
* frame.push();
* s2 = jh::createJString("s2");
*
* // Both strings are again alive.
* // Now lets pop the frame, but leave the string s2 alive:
* frame.pop(&s2);
*
* // Both strings are still alive!
* log(jh::jstringToStdString(s1) + jh::jstringToStdString(s2));
*
* @endcode
*/

#ifndef JH_LOCAL_REFERENCE_FRAME_HPP
#define JH_LOCAL_REFERENCE_FRAME_HPP

#include <jni.h>
#include "../core/JNIEnvironment.hpp"

namespace jh
{
    /**
    * Class that can automatically clear all created local java references.
    */
    class LocalReferenceFrame
    {
    public:
        /**
        * Creates local frame with a capacity of 'frameSize' references. Default is 16.
        */
        LocalReferenceFrame(int frameSize = 16)
        : m_frameSize(frameSize)
        , m_framesCount(0)
        {
            push();
        }

        /**
        * Pops all the created local frames.
        */
        ~LocalReferenceFrame()
        {
            while (pop());
        }

        /**
        * Pushes yet another local frame. The next pop() call will free only
        * the references created after this call and before the pop() call.
        *
        * @return True if new frame was sucessfully created and false otherwise.
        */
        bool push()
        {
            JNIEnv *env = getCurrentJNIEnvironment();

            if (env->PushLocalFrame(m_frameSize) == 0) {
                ++m_framesCount;
                return true;
            }

            return false;
        }

        /**
        * Pops the local frame if there is any and clears all local references
        * that were created after the previous push() call (or the object creation)
        * and before this moment. If there is no active local frame does nothing.
        *
        * @return True if there was an active local frame and it was poped. False otherwise.
        */
        bool pop()
        {
            return pop<jobject>(nullptr);
        }

        /**
        * Pops the local frame if there is any and clears all local references
        * that were created after the previous push() call (or the object creation)
        * and before this moment. If there is no active local frame does nothing.
        *
        * This method also preserves one local reference from the active local frame
        * and updates it's pointer so it remains valid.
        *
        * @param jobjectToKeep A pointer to the jobject pointer that should be preserved after local frame destruction.
        * @return True if there was an active local frame and it was poped. False otherwise.
        */
        template <class JObjectCastable>
        bool pop(JObjectCastable* jobjectToKeep)
        {
            if (m_framesCount) {
                JNIEnv *env = getCurrentJNIEnvironment();

                if (jobjectToKeep) {
                    *jobjectToKeep = static_cast<JObjectCastable>(env->PopLocalFrame(*jobjectToKeep));
                } else {
                    env->PopLocalFrame(nullptr);
                }

                --m_framesCount;

                return true;
            }

            return false;
        }

    private:
        /**
        * The size of created frames.
        */
        int m_frameSize;

        /**
        * The number of active local frames.
        */
        int m_framesCount;

        /**
        * Local frame should not be copied.
        */
        LocalReferenceFrame(const LocalReferenceFrame &) = delete;
        void operator=(const LocalReferenceFrame &) = delete;
    };
}

#endif
