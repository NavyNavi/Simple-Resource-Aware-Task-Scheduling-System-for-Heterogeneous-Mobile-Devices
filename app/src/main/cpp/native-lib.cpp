#include <jni.h>
#include <string>
#include "testcase/printPlan.cpp"
#include "scheduler/profiler.cpp"

enum testcases{wifi_communication, matrix_mul};

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_WiFiDirect_startScheduler(JNIEnv* env, jobject jObj, int testcase) {
    std::string res;
    switch (testcase) {
        case wifi_communication:
            res = getSolution();
            break;
        case matrix_mul:
            break;
        default:
            break;
    }
    Scheduler& scheduler = Scheduler::getInstance();
    scheduler.startScheduler(env, jObj);

    return env->NewStringUTF(res.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_WiFiDirect_executeTask(JNIEnv* env, jobject jObj, jstring serializedTask) {
    jboolean isCopy;
    size_t length = env->GetStringLength(serializedTask);
    const char* cpp_char = env->GetStringUTFChars(serializedTask, &isCopy);
    std::string cpp_string = std::string(cpp_char, length);

    Worker& worker = Worker::getInstance();
    worker.setEnv(env, jObj);
    worker.queueTask(cpp_string);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_WiFiDirect_receiveCompletedTask(JNIEnv* env, jobject jObj, jstring serializedTask) {
    jboolean isCopy;
    size_t length = env->GetStringLength(serializedTask);
    const char* cpp_char = env->GetStringUTFChars(serializedTask, &isCopy);
    std::string cpp_string = std::string(cpp_char, length);

    Profiler& profiler = Profiler::getInstance();
    profiler.updateData(cpp_string);
}