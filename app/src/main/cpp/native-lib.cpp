#include <jni.h>
#include <string>
#include "testcase/printPlan.cpp"
#include "scheduler/profiler.cpp"

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

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_completeTask(JNIEnv* env, jobject jObj, jstring serializedTask) {
    jboolean isCopy;
    size_t length = env->GetStringLength(serializedTask);
    const char* cpp_char = env->GetStringUTFChars(serializedTask, &isCopy);
    std::string cpp_string = std::string(cpp_char, length);

    Profiler& profiler = Profiler::getInstance();
    profiler.updateData(cpp_string);
}