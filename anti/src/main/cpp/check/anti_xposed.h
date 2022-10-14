#ifndef ANTI_ANDROID_ANTI_XPOSED_H
#define ANTI_ANDROID_ANTI_XPOSED_H

#include <jni.h>


enum {
    // no xposed environment.
    NO_XPOSED,
    // have xposed environment but not hooked us,
    // or unknown xposed implementation, so hooks cannot be cleared.
    FOUND_XPOSED,
    // xposed hooks cleared.
    ANTIED_XPOSED,
    // can not clear hooks.
    CAN_NOT_ANTI_XPOSED,
};

class AntiXposed {
public:
    int get_xposed_status(JNIEnv *env, int sdk);

public:

};


#endif //ANTI_ANDROID_ANTI_XPOSED_H
