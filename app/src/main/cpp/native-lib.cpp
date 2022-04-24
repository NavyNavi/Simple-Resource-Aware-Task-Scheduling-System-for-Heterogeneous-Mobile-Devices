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
Java_com_example_myapplication_MainActivity_printPlan(JNIEnv* env, jobject ) {
    std::string res = getSolution();
    return env->NewStringUTF(res.c_str());
}