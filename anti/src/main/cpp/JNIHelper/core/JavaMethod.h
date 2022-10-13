#ifndef ANTI_ANDROID_JAVAMETHOD_H
#define ANTI_ANDROID_JAVAMETHOD_H
namespace jh {
    template<class ReturnType, class ... ArgumentTypes>
    jmethodID findMethod(std::string className, std::string methodName) {
        JNIEnv *env = getCurrentJNIEnvironment();
        jclass javaClass = FindClass(className);
        if (javaClass == nullptr)
            return nullptr;
        std::string methodSignature = getJavaMethodSignature<ReturnType, ArgumentTypes...>();
        jmethodID javaMethod = env->GetMethodID(javaClass, methodName.c_str(),
                                                methodSignature.c_str());
        env->DeleteLocalRef(javaClass);

        return javaMethod;
    }
}
#endif //ANTI_ANDROID_JAVAMETHOD_H
