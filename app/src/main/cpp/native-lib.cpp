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

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_getTask(JNIEnv* env, jobject ) {
    std::string res = getSolution();

    jclass taskClass = env->FindClass("com/example/myapplication/Task");
    jmethodID methodId = env->GetMethodID(taskClass, "<init>", "()V");
    jobject sampleObj = env->NewObject(taskClass, methodId);


    return env->NewStringUTF(res.c_str());
}