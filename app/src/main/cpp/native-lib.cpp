#include <jni.h>
#include <string>
#include "testcase/printPlan.cpp"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_printPlan(JNIEnv* env, jobject jObj) {
    std::string res = getSolution();
    jstring jTask = env->NewStringUTF(res.c_str());
    jclass taskClass = env->FindClass("com/example/myapplication/MainActivity");
    jmethodID methodId = env->GetMethodID(taskClass, "setSerializedTask", "(Ljava/lang/String;)V");
    env->CallVoidMethod(jObj, methodId, jTask);

    return env->NewStringUTF(res.c_str());
}