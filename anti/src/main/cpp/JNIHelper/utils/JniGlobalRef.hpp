#ifndef UTDID_JNIGLOBALREF_HPP
#define UTDID_JNIGLOBALREF_HPP


#include "../core/JNIEnvironment.hpp"
#include "../core/JavaCustomClass.hpp"

namespace jh {
    template<typename JniType>
    class JniGlobalRef {
    public:
        JniGlobalRef() : _obj(nullptr) {}

        JniGlobalRef(const JniGlobalRef<JniType> &ref) : _obj(nullptr) { set(ref.get()); }

        ~JniGlobalRef() { set(nullptr); }

        JniType get() const { return _obj; }

        void set(JniType obj) {
            JNIEnv *env = getCurrentJNIEnvironment();
            if (_obj) {
                if (env) {
                    env->DeleteGlobalRef(_obj);
                }
                _obj = nullptr;
            }
            if (obj && env) {
                _obj = (JniType) env->NewGlobalRef(obj);
            }
        }

        operator JniType() const { return _obj; }

        void operator=(const JniType other) { set(other); }

        void operator=(const JniGlobalRef<JniType> &ref) { set(ref.get()); }


        operator bool() const {
            return !areEqual(get(), nullptr);
        }

    private:
        JniType _obj;
    };
}

#endif //UTDID_JNIGLOBALREF_HPP
